modded class PlayerBase
{
	protected float m_ZenDirtyDirtiness;
	protected bool m_ZenDirtyFliesActive;

	protected ref Timer m_ZenDirtyFliesCheckTimer;
	protected ref Timer m_ZenDirtyFliesStopTimer;

	protected ref EffectParticle m_ZenDirtyFliesEffect;
	protected int m_ZenDirtyFliesEffectId = -1;
	protected ref EffectSound m_ZenDirtyFliesSound;

	protected ref Timer m_ZenDirtyPassiveTimer;
	static const float ZEN_DIRTY_PASSIVE_TICK_SECONDS = 30.0;

	void PlayerBase()
	{
		RegisterNetSyncVariableFloat("m_ZenDirtyDirtiness", 0.0, ZEN_DIRTY_MAX, 1);
		RegisterNetSyncVariableBool("m_ZenDirtyFliesActive");
	}

	float ZenDirty_GetDirtiness()
	{
		return m_ZenDirtyDirtiness;
	}

	bool ZenDirty_IsDirty()
	{
		return m_ZenDirtyDirtiness > 0.0;
	}

	bool ZenDirty_IsFullDirty()
	{
		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled)
			return false;

		return m_ZenDirtyDirtiness >= config.FullDirtyThreshold;
	}

	protected void ZenDirty_StartPassiveDirtTimer()
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		if (!GetZenDirtyConfig().Enabled)
			return;

		if (GetZenDirtyConfig().SecondsUntilFullDirty <= 0.0)
			return;

		if (!m_ZenDirtyPassiveTimer)
			m_ZenDirtyPassiveTimer = new Timer(CALL_CATEGORY_SYSTEM);

		if (m_ZenDirtyPassiveTimer.IsRunning())
			return;

		m_ZenDirtyPassiveTimer.Run(ZEN_DIRTY_PASSIVE_TICK_SECONDS, this, "ZenDirty_OnPassiveDirtTick", NULL, true);
	}

	protected void ZenDirty_OnPassiveDirtTick()
	{
		if (!g_Game)
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled)
			return;

		if (ZenDirty_GetDirtiness() >= ZEN_DIRTY_MAX)
			return;

		float dirtinessPerSecond = ZEN_DIRTY_MAX / config.SecondsUntilFullDirty;
		float dirtinessToAdd = dirtinessPerSecond * ZEN_DIRTY_PASSIVE_TICK_SECONDS;

		if (dirtinessToAdd <= 0.0)
			return;

		ZenDirty_AddDirtiness(dirtinessToAdd);
	}

	void ZenDirty_AddDirtiness(float amount)
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		if (amount <= 0.0)
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled)
			return;

		ZenDirty_SetDirtiness(m_ZenDirtyDirtiness + amount);

		if (GetZenDirtyConfig().DebugMode)
		{
			ZenFunctions.SendPlayerMessage(this, "Applied dirt=" + amount + " currentDirt=" + m_ZenDirtyDirtiness + "%");
		}
	}

	void ZenDirty_SetDirtiness(float value)
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		if (!GetZenDirtyConfig().Enabled)
			return;

		bool wasFullDirty = ZenDirty_IsFullDirty();
		float newValue = Math.Clamp(value, 0.0, ZEN_DIRTY_MAX);

		if (newValue == m_ZenDirtyDirtiness)
			return;

		m_ZenDirtyDirtiness = newValue;
		SetSynchDirty();

		bool isFullDirty = ZenDirty_IsFullDirty();

		if (!wasFullDirty && isFullDirty)
		{
			if (GetZenDirtyConfig().FliesAlwaysTriggerOnFullDirty)
				ZenDirty_StartFliesServer();

			ZenDirty_ScheduleNextFliesCheck();
		}
		else
		if (wasFullDirty && !isFullDirty)
		{
			ZenDirty_StopFliesSystem();
		}
	}

	void ZenDirty_CleanAmount(float amount, bool applyColdEffect = true)
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		if (amount <= 0.0)
			return;

		float dirtinessBefore = ZenDirty_GetDirtiness();

		if (dirtinessBefore <= 0.0)
			return;

		ZenDirty_SetDirtiness(dirtinessBefore - amount);

		if (applyColdEffect && ZenDirty_GetDirtiness() < dirtinessBefore)
			ZenDirty_ApplyPostWashTemperature();

		if (GetZenDirtyConfig().DebugMode)
		{
			ZenFunctions.SendPlayerMessage(this, "Applied clean=" + amount + "% coldEffect=" + applyColdEffect + " currentDirt=" + m_ZenDirtyDirtiness + "%");
		}
	}

	protected void ZenDirty_ApplyPostWashTemperature()
	{
		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config)
			return;

		if (config.WashingRemovesHeatBuffer)
		{
			PlayerStat<float> heatBuffer = GetStatHeatBuffer();
			if (heatBuffer)
				heatBuffer.Set(0.0);

			ModifiersManager modifiers = GetModifiersManager();
			if (modifiers && modifiers.IsModifierActive(eModifiers.MDF_HEATBUFFER))
				modifiers.DeactivateModifier(eModifiers.MDF_HEATBUFFER);

			ToggleHeatBufferVisibility(0);
		}

		PlayerStat<float> heatComfort = GetStatHeatComfort();
		if (heatComfort)
		{
			float coldHeatComfort = Math.Clamp(config.WashColdHeatComfort, heatComfort.GetMin(), heatComfort.GetMax());
			heatComfort.Set(coldHeatComfort);
		}
	}

	void ZenDirty_ClearBloodyHands()
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		if (!HasBloodyHands())
			return;

		PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		if (moduleLifespan)
			moduleLifespan.UpdateBloodyHandsVisibilityEx(this, eBloodyHandsTypes.CLEAN);

		ClearBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA);
	}

	override float GetImmunity()
	{
		float immunity = super.GetImmunity();

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled)
			return immunity;

		if (!ZenDirty_IsFullDirty())
			return immunity;

		return Math.Clamp(immunity * config.FullDirtyImmunityMultiplier, 0.0, 1.0);
	}

	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionZenDirtyWashSelfWater, InputActionMap);
	}

	override void OnCommandSwimStart()
	{
		super.OnCommandSwimStart();

		if (!g_Game || !g_Game.IsServer())
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled || !config.SwimmingCleans)
			return;

		ZenDirty_CleanAmount(ZEN_DIRTY_MAX, false);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		ZenDirty_UpdateClientFlies();
		ZenDirty_RefreshHandsVisual();
	}

	override void OnPlayerLoaded()
	{
		super.OnPlayerLoaded();

		if (g_Game && g_Game.IsServer() && !ZenDirty_IsAI())
			ZenDirty_StartPassiveDirtTimer();

		ZenDirty_RefreshHandsVisual();
	}

	bool ZenDirty_IsAI() 
	{
		#ifdef DZ_Expansion_AI
		if (IsAI())
			return true;
		#endif

		return GetType().Contains("eAI");
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		if (slot_name == "Gloves")
			ZenDirty_RefreshHandsVisual();
	}

	protected void ZenDirty_ScheduleNextFliesCheck()
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled || !config.FliesEnabled || !ZenDirty_IsFullDirty())
			return;

		if (!m_ZenDirtyFliesCheckTimer)
			m_ZenDirtyFliesCheckTimer = new Timer(CALL_CATEGORY_SYSTEM);

		if (m_ZenDirtyFliesCheckTimer.IsRunning())
			m_ZenDirtyFliesCheckTimer.Stop();
		
		float delay = Math.RandomFloatInclusive(config.FliesCheckIntervalMinSeconds, config.FliesCheckIntervalMaxSeconds);
		m_ZenDirtyFliesCheckTimer.Run(delay, this, "ZenDirty_OnFliesCheck", NULL, false);
	}

	protected void ZenDirty_OnFliesCheck()
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled || !config.FliesEnabled || !ZenDirty_IsFullDirty())
		{
			ZenDirty_StopFliesSystem();
			return;
		}

		if (!m_ZenDirtyFliesActive)
		{
			float roll = Math.RandomFloatInclusive(0.0, 100.0);
			if (roll <= config.FliesChancePercent)
				ZenDirty_StartFliesServer();
		}

		ZenDirty_ScheduleNextFliesCheck();
	}

	protected void ZenDirty_StartFliesServer()
	{
		if (m_ZenDirtyFliesActive)
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled || !config.FliesEnabled || !ZenDirty_IsFullDirty())
			return;

		m_ZenDirtyFliesActive = true;
		SetSynchDirty();

		if (!m_ZenDirtyFliesStopTimer)
			m_ZenDirtyFliesStopTimer = new Timer(CALL_CATEGORY_SYSTEM);

		if (m_ZenDirtyFliesStopTimer.IsRunning())
			m_ZenDirtyFliesStopTimer.Stop();

		float duration = Math.RandomFloatInclusive(config.FliesDurationMinSeconds, config.FliesDurationMaxSeconds);
		m_ZenDirtyFliesStopTimer.Run(duration, this, "ZenDirty_StopFliesServer", NULL, false);
	}

	protected void ZenDirty_StopFliesServer()
	{
		if (!m_ZenDirtyFliesActive)
			return;

		m_ZenDirtyFliesActive = false;
		SetSynchDirty();
	}

	protected void ZenDirty_StopFliesSystem()
	{
		if (m_ZenDirtyFliesCheckTimer && m_ZenDirtyFliesCheckTimer.IsRunning())
			m_ZenDirtyFliesCheckTimer.Stop();

		if (m_ZenDirtyFliesStopTimer && m_ZenDirtyFliesStopTimer.IsRunning())
			m_ZenDirtyFliesStopTimer.Stop();

		ZenDirty_StopFliesServer();
	}

	protected void ZenDirty_UpdateClientFlies()
	{
		if (!g_Game || g_Game.IsDedicatedServer())
			return;

		if (!GetZenDirtyConfig().Enabled)
		{
			ZenDirty_StopClientFlies();
			return;
		}

		if (m_ZenDirtyFliesActive)
		{
			ZenDirty_StartClientFlies();
		}
		else
		{
			ZenDirty_StopClientFlies();
		}
	}

	protected void ZenDirty_StartClientFlies()
	{
		if (m_ZenDirtyFliesEffect && SEffectManager.IsEffectExist(m_ZenDirtyFliesEffectId))
			return;

		m_ZenDirtyFliesEffect = new EffSwarmingFlies();
		if (!m_ZenDirtyFliesEffect)
			return;

		m_ZenDirtyFliesEffect.SetDecalOwner(this);
		m_ZenDirtyFliesEffectId = SEffectManager.PlayOnObject(m_ZenDirtyFliesEffect, this, "0 0.25 0");

		Particle particle = m_ZenDirtyFliesEffect.GetParticle();
		if (particle)
		{
			int boneIndex = GetBoneIndexByName("Spine2");
			if (boneIndex != -1)
				AddChild(particle, boneIndex);
		}

		if (!m_ZenDirtyFliesSound)
			PlaySoundSetLoop(m_ZenDirtyFliesSound, ZEN_DIRTY_FLIES_SOUNDSET, 1.0, 1.0);
	}

	protected void ZenDirty_StopClientFlies()
	{
		if (m_ZenDirtyFliesEffect)
		{
			SEffectManager.DestroyEffect(m_ZenDirtyFliesEffect);
			m_ZenDirtyFliesEffect = null;
			m_ZenDirtyFliesEffectId = -1;
		}

		if (m_ZenDirtyFliesSound)
		{
			StopSoundSet(m_ZenDirtyFliesSound);
			m_ZenDirtyFliesSound = null;
		}
	}

	protected string ZenDirty_GetDirtyHandsMaterial()
	{
		string normalMaterial;
		string configPath = "CfgVehicles " + GetPlayerClass() + " BloodyHands mat_normal";

		g_Game.ConfigGetText(configPath, normalMaterial);

		if (normalMaterial == "")
			return "";

		normalMaterial.ToLower();

		if (!normalMaterial.Contains("dz\\characters\\heads\\data\\"))
			return "";

		normalMaterial.Replace("dz\\characters\\heads\\data\\", "ZenDirty\\data\\hands\\");
		normalMaterial.Replace(".rvmat", "_dirty.rvmat");

		return normalMaterial;
	}

	bool ZenDirty_CanWash(int waterType) 
	{
		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config)
			return false;

		if (!config.Enabled)
			return false;

		float thresh = 0;

		if (waterType == ZEN_DIRTY_SRC_WELL)
		{
			thresh = config.WashDirtyWellsThreshold;
		} else 
		if (waterType == ZEN_DIRTY_SRC_WATER)
		{
			thresh = config.WashDirtyWaterSourceThreshold;
		} else 
		if (waterType == ZEN_DIRTY_SRC_CONTAINER)
		{
			thresh = config.WashDirtyContainerThreshold;
		}

		if (ZenDirty_GetDirtiness() >= thresh)
			return true;

		return false;
	}

	void ZenDirty_RefreshHandsVisual()
	{
		if (!g_Game || g_Game.IsDedicatedServer())
			return;

		if (FindAttachmentBySlotName("Gloves"))
			return;

		int glovesSlotId = InventorySlots.GetSlotIdFromString("Gloves");
		EntityAI handsPlaceholder = GetInventory().FindPlaceholderForSlot(glovesSlotId);

		if (!handsPlaceholder)
			return;

		// Vanilla bloody hands always take priority.
		if (HasBloodyHands())
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (config && config.Enabled && config.DirtyHandsEnabled && ZenDirty_GetDirtiness() >= config.DirtyHandsThreshold)
		{
			string dirtyMaterial = ZenDirty_GetDirtyHandsMaterial();

			if (dirtyMaterial != "")
			{
				handsPlaceholder.SetObjectMaterial(0, dirtyMaterial);
				return;
			}
		}

		ZenDirty_RestoreNormalHandsMaterial(handsPlaceholder);
	}

	protected void ZenDirty_RestoreNormalHandsMaterial(EntityAI handsPlaceholder)
	{
		if (!handsPlaceholder)
			return;

		string normalMaterial;
		string normalMaterialPath = "CfgVehicles " + GetPlayerClass() + " BloodyHands mat_normal";

		g_Game.ConfigGetText(normalMaterialPath, normalMaterial);

		if (normalMaterial != "")
			handsPlaceholder.SetObjectMaterial(0, normalMaterial);
	}

	override void CF_OnStoreSave(CF_ModStorageMap storage)
	{
		super.CF_OnStoreSave(storage);

		auto ctx = storage[ZEN_DIRTY_STORAGE];
		if (!ctx)
			return;

		ctx.Write(m_ZenDirtyDirtiness);
	}

	override bool CF_OnStoreLoad(CF_ModStorageMap storage)
	{
		if (!super.CF_OnStoreLoad(storage))
			return false;

		auto ctx = storage[ZEN_DIRTY_STORAGE];
		if (!ctx)
			return true;

		if (ctx.GetVersion() >= 1)
		{
			if (!ctx.Read(m_ZenDirtyDirtiness))
				return false;

			m_ZenDirtyDirtiness = Math.Clamp(m_ZenDirtyDirtiness, 0.0, ZEN_DIRTY_MAX);
		}

		if (g_Game && g_Game.IsServer())
		{
			SetSynchDirty();

			if (ZenDirty_IsFullDirty())
			{
				if (GetZenDirtyConfig().FliesAlwaysTriggerOnFullDirty)
					ZenDirty_StartFliesServer();

				ZenDirty_ScheduleNextFliesCheck();
			}
		}

		return true;
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_ZenDirtyFliesCheckTimer && m_ZenDirtyFliesCheckTimer.IsRunning())
			m_ZenDirtyFliesCheckTimer.Stop();

		if (m_ZenDirtyFliesStopTimer && m_ZenDirtyFliesStopTimer.IsRunning())
			m_ZenDirtyFliesStopTimer.Stop();

		if (m_ZenDirtyPassiveTimer && m_ZenDirtyPassiveTimer.IsRunning())
			m_ZenDirtyPassiveTimer.Stop();

		ZenDirty_StopClientFlies();
		super.EEDelete(parent);
	}
}
