ref ZenDirtyConfig g_ZenDirtyConfig;

static ZenDirtyConfig GetZenDirtyConfig()
{
	if (!g_ZenDirtyConfig)
		GetZenConfigRegister().RegisterConfig(ZenDirtyConfig);

	return g_ZenDirtyConfig;
}

modded class ZenConfigRegister
{
	override void RegisterPreload()
	{
		super.RegisterPreload();
		RegisterType(ZenDirtyConfig);
	}
}

class ZenDirtyClientConfigPayload
{
	bool Enabled;
	float FullDirtyThreshold;

	bool ShowDirtyBadge;
	string DirtyBadgeTexture;

	float WashDirtyWellsThreshold;
	float WashDirtyWaterSourceThreshold;
	float WashDirtyContainerThreshold;

	bool WashingAtWellsEnabled;
	bool WashingAtWaterSourcesEnabled;
	float WashWellActionSeconds;
	float WashWaterSourcesActionSeconds;

	bool WashingFromContainersEnabled;
	float WashPercentPerLitre;
	float ContainerWashActionSeconds;

	bool DirtyHandsEnabled;
	float DirtyHandsThreshold;
}

class ZenDirtyConfig : ZenConfigBase
{
	bool Enabled;
	bool DebugMode;
	float FullDirtyThreshold;
	float FullDirtyImmunityMultiplier;

	float SecondsUntilFullDirty;

	bool ShowDirtyBadge;
	string DirtyBadgeTexture;

	bool FliesEnabled;
	float FliesChancePercent;
	float FliesCheckIntervalMinSeconds;
	float FliesCheckIntervalMaxSeconds;
	float FliesDurationMinSeconds;
	float FliesDurationMaxSeconds;

	float WashDirtyWellsThreshold;
	float WashDirtyWaterSourceThreshold;
	float WashDirtyContainerThreshold;

	bool WashingAtWellsEnabled;
	bool WashingAtWaterSourcesEnabled;
	bool WashingFromContainersEnabled;
	
	float WashWellActionSeconds;
	float WashWaterSourcesActionSeconds;
	float WashPercentPerLitre;
	float ContainerWashActionSeconds;

	bool WashingRemovesHeatBuffer;
	float WashColdHeatComfort;

	bool SwimmingCleans;

	bool DirtyHandsEnabled;
	float DirtyHandsThreshold;

	ref map<string, float> ActionDirtiness;
	ref map<string, float> RecipeDirtiness;

	override void OnRegistered()
	{
		g_ZenDirtyConfig = this;
	}

	override string GetCurrentVersion()
	{
		return "1.29.2";
	}

	override bool ShouldLoadOnServer()
	{
		return true;
	}

	override bool ShouldLoadOnClient()
	{
		return false;
	}

	override bool ShouldSyncToClient()
	{
		return true;
	}

	override bool ShouldSaveOnShutdown()
	{
		return false;
	}

	override bool IsClientOnlyConfig()
	{
		return false;
	}

	override bool IsServerOnlyConfig()
	{
		return false;
	}

	override bool ShouldDebugPrint()
	{
		return false;
	}

	override void SetDefaults()
	{
		Enabled = true;
		DebugMode = false;
		FullDirtyThreshold = 100.0;
		FullDirtyImmunityMultiplier = 0.5;

		SecondsUntilFullDirty = 0.0;

		ShowDirtyBadge = true;
		DirtyBadgeTexture = ZEN_DIRTY_DEFAULT_BADGE_TEXTURE;

		FliesEnabled = true;
		FliesChancePercent = 75.0;
		FliesCheckIntervalMinSeconds = 600.0;
		FliesCheckIntervalMaxSeconds = 900.0;
		FliesDurationMinSeconds = 60.0;
		FliesDurationMaxSeconds = 120.0;

		WashDirtyWellsThreshold = 10.0;
		WashDirtyWaterSourceThreshold = 10.0;
		WashDirtyContainerThreshold = 10.0;

		WashingAtWellsEnabled = true;
		WashingAtWaterSourcesEnabled = true;

		// These are now the time required to wash 100 dirtiness points.
		WashWellActionSeconds = 60.0;
		WashWaterSourcesActionSeconds = 30.0;

		WashingFromContainersEnabled = true;

		// 1 litre removes 10 dirtiness points by default.
		WashPercentPerLitre = 25.0;

		// Time required to clean 100 dirtiness points with container water.
		// 40 seconds means a 1L canteen at 10%/L takes 4 seconds.
		ContainerWashActionSeconds = 60.0;

		WashingRemovesHeatBuffer = true;
		WashColdHeatComfort = -0.5;

		SwimmingCleans = true;

		DirtyHandsEnabled = true;
		DirtyHandsThreshold = 50.0;

		ActionDirtiness = new map<string, float>;
		ActionDirtiness.Insert("ActionDigInStash", 5.0);
		ActionDirtiness.Insert("ActionDigOutStash", 5.0);
		ActionDirtiness.Insert("ActionDigGardenPlot", 10.0);
		ActionDirtiness.Insert("ActionDismantleGardenPlot", 10.0);
		ActionDirtiness.Insert("ActionDigWorms", 2.5);
		ActionDirtiness.Insert("ActionBuryBody", 50.0);
		ActionDirtiness.Insert("ActionBuryAshes", 50.0);
		ActionDirtiness.Insert("ActionFertilizeSlot", 1.0);
		ActionDirtiness.Insert("ActionPlantSeed", 1.0);
		ActionDirtiness.Insert("ActionWaterGardenSlot", 1.0);
		ActionDirtiness.Insert("ActionWaterPlant", 1.0);
		ActionDirtiness.Insert("ActionHarvestCrops", 2.0);
		ActionDirtiness.Insert("ActionSkinning", 15.0);
		ActionDirtiness.Insert("ActionBuildPart", 2.5);
		ActionDirtiness.Insert("ActionDismantlePart", 2.5);
		ActionDirtiness.Insert("ActionDestroyPart", 2.5);

		RecipeDirtiness = new map<string, float>;
		RecipeDirtiness.Insert("PrepareAnimal", 10.0);
		RecipeDirtiness.Insert("PrepareFish", 5.0);
	}

	override void AfterLoad()
	{
		Validate();
	}

	override void AfterConfigReceived()
	{
		super.AfterConfigReceived();
		Validate();
	}

	void Validate()
	{
		FullDirtyThreshold = Math.Clamp(FullDirtyThreshold, 1.0, ZEN_DIRTY_MAX);
		FullDirtyImmunityMultiplier = Math.Clamp(FullDirtyImmunityMultiplier, 0.0, 1.0);

		SecondsUntilFullDirty = Math.Max(SecondsUntilFullDirty, 0.0);

		FliesChancePercent = Math.Clamp(FliesChancePercent, 0.0, 100.0);
		FliesCheckIntervalMinSeconds = Math.Max(FliesCheckIntervalMinSeconds, 1.0);
		FliesCheckIntervalMaxSeconds = Math.Max(FliesCheckIntervalMaxSeconds, FliesCheckIntervalMinSeconds);
		FliesDurationMinSeconds = Math.Max(FliesDurationMinSeconds, 1.0);
		FliesDurationMaxSeconds = Math.Max(FliesDurationMaxSeconds, FliesDurationMinSeconds);

		WashDirtyWellsThreshold = Math.Clamp(WashDirtyWellsThreshold, 0.0, ZEN_DIRTY_MAX);
		WashDirtyWaterSourceThreshold = Math.Clamp(WashDirtyWaterSourceThreshold, 0.0, ZEN_DIRTY_MAX);
		WashDirtyContainerThreshold = Math.Clamp(WashDirtyContainerThreshold, 0.0, ZEN_DIRTY_MAX);

		WashWellActionSeconds = Math.Max(WashWellActionSeconds, 0.1);
		WashWaterSourcesActionSeconds = Math.Max(WashWaterSourcesActionSeconds, 0.1);

		WashPercentPerLitre = Math.Clamp(WashPercentPerLitre, 0.0, 100.0);
		ContainerWashActionSeconds = Math.Max(ContainerWashActionSeconds, 0.1);

		WashColdHeatComfort = Math.Clamp(WashColdHeatComfort, -1.0, 0.0);

		DirtyHandsThreshold = Math.Clamp(DirtyHandsThreshold, 0.0, ZEN_DIRTY_MAX);

		if (DirtyBadgeTexture == "")
			DirtyBadgeTexture = ZEN_DIRTY_DEFAULT_BADGE_TEXTURE;

		if (!ActionDirtiness)
			ActionDirtiness = new map<string, float>;

		if (!RecipeDirtiness)
			RecipeDirtiness = new map<string, float>;

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenDirty"))
			Enabled = false;
		#endif
	}

	override bool ReadJson(string path, out string err)
	{
		return JsonFileLoader<ZenDirtyConfig>.LoadFile(path, this, err);
	}

	override bool WriteJson(string path, out string err)
	{
		return JsonFileLoader<ZenDirtyConfig>.SaveFile(path, this, err);
	}

	override protected bool BuildSyncPayload(out string payload, out string err)
	{
		ZenDirtyClientConfigPayload syncData = new ZenDirtyClientConfigPayload;

		syncData.Enabled = Enabled;
		syncData.FullDirtyThreshold = FullDirtyThreshold;

		syncData.ShowDirtyBadge = ShowDirtyBadge;
		syncData.DirtyBadgeTexture = DirtyBadgeTexture;

		syncData.WashDirtyWellsThreshold = WashDirtyWellsThreshold;
		syncData.WashDirtyWaterSourceThreshold = WashDirtyWaterSourceThreshold;
		syncData.WashDirtyContainerThreshold = WashDirtyContainerThreshold;

		syncData.WashingAtWellsEnabled = WashingAtWellsEnabled;
		syncData.WashingAtWaterSourcesEnabled = WashingAtWaterSourcesEnabled;
		syncData.WashWellActionSeconds = WashWellActionSeconds;
		syncData.WashWaterSourcesActionSeconds = WashWaterSourcesActionSeconds;

		syncData.WashingFromContainersEnabled = WashingFromContainersEnabled;
		syncData.WashPercentPerLitre = WashPercentPerLitre;
		syncData.ContainerWashActionSeconds = ContainerWashActionSeconds;

		syncData.DirtyHandsEnabled = DirtyHandsEnabled;
		syncData.DirtyHandsThreshold = DirtyHandsThreshold;

		return JsonFileLoader<ZenDirtyClientConfigPayload>.MakeData(syncData, payload, err, false);
	}

	override protected bool ApplySyncPayload(string payload, out string err)
	{
		ZenDirtyClientConfigPayload syncData = new ZenDirtyClientConfigPayload;

		if (!JsonFileLoader<ZenDirtyClientConfigPayload>.LoadData(payload, syncData, err))
			return false;

		Enabled = syncData.Enabled;
		FullDirtyThreshold = syncData.FullDirtyThreshold;

		ShowDirtyBadge = syncData.ShowDirtyBadge;
		DirtyBadgeTexture = syncData.DirtyBadgeTexture;

		WashDirtyWellsThreshold = syncData.WashDirtyWellsThreshold;
		WashDirtyWaterSourceThreshold = syncData.WashDirtyWaterSourceThreshold;
		WashDirtyContainerThreshold = syncData.WashDirtyContainerThreshold;

		WashingAtWellsEnabled = syncData.WashingAtWellsEnabled;
		WashingAtWaterSourcesEnabled = syncData.WashingAtWaterSourcesEnabled;
		WashWellActionSeconds = syncData.WashWellActionSeconds;
		WashWaterSourcesActionSeconds = syncData.WashWaterSourcesActionSeconds;

		WashingFromContainersEnabled = syncData.WashingFromContainersEnabled;
		WashPercentPerLitre = syncData.WashPercentPerLitre;
		ContainerWashActionSeconds = syncData.ContainerWashActionSeconds;

		DirtyHandsEnabled = syncData.DirtyHandsEnabled;
		DirtyHandsThreshold = syncData.DirtyHandsThreshold;

		Validate();

		return true;
	}
}