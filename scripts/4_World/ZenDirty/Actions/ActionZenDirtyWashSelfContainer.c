class ActionZenDirtyWashSelfContainerCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float washTime = 0.1;

		if (m_ActionData && m_ActionData.m_Player && m_ActionData.m_MainItem)
		{
			ZenDirtyConfig config = GetZenDirtyConfig();

			if (config)
			{
				float currentDirtiness = Math.Clamp(m_ActionData.m_Player.ZenDirty_GetDirtiness(), 0.0, ZEN_DIRTY_MAX);
				float litresAvailable = m_ActionData.m_MainItem.GetQuantity() / 1000.0;
				float maximumCleanAmount = litresAvailable * config.WashPercentPerLitre;
				float dirtinessToClean = Math.Min(currentDirtiness, maximumCleanAmount);

				washTime = config.ContainerWashActionSeconds * (dirtinessToClean / ZEN_DIRTY_MAX);
				washTime = Math.Max(washTime, 0.1);
			}
		}

		m_ActionData.m_ActionComponent = new CAContinuousTime(washTime);
	}
}

class ActionZenDirtyWashSelfContainer : ActionContinuousBase
{
	void ActionZenDirtyWashSelfContainer()
	{
		m_CallbackClass = ActionZenDirtyWashSelfContainerCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CLEANHANDSBOTTLE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_CLEANHANDSBOTTLE;
		m_FullBody = false;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#STR_ZENDIRTY_ACTION_WASH_SELF";
	}

	override ActionData CreateActionData()
	{
		ZenDirtyWashActionData data = new ZenDirtyWashActionData;
		return data;
	}

	override bool HasProneException()
	{
		return true;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !item)
			return false;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled || !config.WashingFromContainersEnabled)
			return false;

		if (config.WashPercentPerLitre <= 0.0)
			return false;

		if (!player.ZenDirty_IsDirty())
			return false;

		if (!item.IsInherited(Bottle_Base))
			return false;

		if (!item.IsLiquidPresent())
			return false;

		if ((item.GetLiquidType() & LIQUID_GROUP_WATER) == 0)
			return false;

		if (item.GetQuantity() <= 0.0)
			return false;

		return player.ZenDirty_CanWash(ZEN_DIRTY_SRC_CONTAINER);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		if (!action_data || !action_data.m_Player || !action_data.m_MainItem)
			return;

		ZenDirtyWashActionData washActionData = ZenDirtyWashActionData.Cast(action_data);

		if (!washActionData)
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled || !config.WashingFromContainersEnabled)
			return;

		if (config.WashPercentPerLitre <= 0.0)
			return;

		ItemBase item = action_data.m_MainItem;

		if (!item.IsLiquidPresent())
			return;

		if ((item.GetLiquidType() & LIQUID_GROUP_WATER) == 0)
			return;

		float currentDirtiness = action_data.m_Player.ZenDirty_GetDirtiness();
		float litresAvailable = item.GetQuantity() / 1000.0;
		float maximumCleanAmount = litresAvailable * config.WashPercentPerLitre;

		washActionData.m_ZenDirtyStartDirtiness = currentDirtiness;
		washActionData.m_ZenDirtyMaximumCleanAmount = Math.Min(currentDirtiness, maximumCleanAmount);
	}

	override void OnEndServer(ActionData action_data)
	{
		float progress = 0.0;
		bool completed = false;

		if (action_data)
		{
			completed = action_data.m_State == UA_FINISHED;

			if (action_data.m_ActionComponent)
				progress = Math.Clamp(action_data.m_ActionComponent.GetProgress(), 0.0, 1.0);
		}

		super.OnEndServer(action_data);

		if (!action_data || !action_data.m_Player || !action_data.m_MainItem)
			return;

		if (progress <= 0.0)
			return;

		ZenDirtyWashActionData washActionData = ZenDirtyWashActionData.Cast(action_data);

		if (!washActionData)
			return;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled || !config.WashingFromContainersEnabled)
			return;

		if (config.WashPercentPerLitre <= 0.0)
			return;

		ItemBase item = action_data.m_MainItem;

		if (!item.IsLiquidPresent())
			return;

		if ((item.GetLiquidType() & LIQUID_GROUP_WATER) == 0)
			return;

		float availableWater = item.GetQuantity();

		if (availableWater <= 0.0)
			return;

		float dirtinessCleaned = washActionData.m_ZenDirtyMaximumCleanAmount * progress;

		float maximumCleanFromCurrentWater = (availableWater / 1000.0) * config.WashPercentPerLitre;
		dirtinessCleaned = Math.Min(dirtinessCleaned, maximumCleanFromCurrentWater);
		dirtinessCleaned = Math.Min(dirtinessCleaned, action_data.m_Player.ZenDirty_GetDirtiness());

		if (dirtinessCleaned <= 0.0)
			return;

		float litresUsed = dirtinessCleaned / config.WashPercentPerLitre;
		float quantityUsed = litresUsed * 1000.0;

		quantityUsed = Math.Min(quantityUsed, availableWater);

		item.AddQuantity(-quantityUsed, false);

		action_data.m_Player.ZenDirty_CleanAmount(dirtinessCleaned, true);

		if (completed)
			action_data.m_Player.ZenDirty_ClearBloodyHands();
	}
}