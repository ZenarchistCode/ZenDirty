class ZenDirtyWashActionData : ActionData
{
	float m_ZenDirtyStartDirtiness;
	float m_ZenDirtyMaximumCleanAmount;
}

class ZenDirtyActionRules
{
	static float GetDirtiness(ActionBase action)
	{
		if (!action)
			return 0.0;

		ZenDirtyConfig config = GetZenDirtyConfig();
		if (!config || !config.Enabled || !config.ActionDirtiness)
			return 0.0;

		string actionClass = action.ClassName();

		float dirtiness;
		if (config.ActionDirtiness.Find(actionClass, dirtiness))
			return Math.Max(dirtiness, 0.0);

		float inheritedDirtiness = 0.0;

		for (int i = 0; i < config.ActionDirtiness.Count(); i++)
		{
			string configuredClass = config.ActionDirtiness.GetKey(i);
			typename configuredType = configuredClass.ToType();

			if (!configuredType)
				continue;

			if (!action.IsInherited(configuredType))
				continue;

			float configuredDirtiness = config.ActionDirtiness.GetElement(i);

			if (configuredDirtiness > inheritedDirtiness)
				inheritedDirtiness = configuredDirtiness;
		}

		return Math.Max(inheritedDirtiness, 0.0);
	}

	static void Process(ActionBase action, ActionData action_data)
	{
		if (!g_Game || !g_Game.IsServer())
			return;

		if (!action || !action_data || !action_data.m_Player)
			return;

		float dirtiness = GetDirtiness(action);
		if (dirtiness <= 0.0)
			return;

		action_data.m_Player.ZenDirty_AddDirtiness(dirtiness);
	}
}

modded class ActionBase
{
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);

		if (g_Game.IsServer() && IsInstant())
			ZenDirtyActionRules.Process(this, action_data);
	}

	override void OnEnd(ActionData action_data)
	{
		super.OnEnd(action_data);

		if (g_Game.IsServer() && !IsInstant() && !IsInherited(AnimatedActionBase))
			ZenDirtyActionRules.Process(this, action_data);
	}
}

modded class ActionContinuousBase
{
	override void OnFinishProgress(ActionData action_data)
	{
		super.OnFinishProgress(action_data);

		if (g_Game.IsServer())
			ZenDirtyActionRules.Process(this, action_data);
	}
}

modded class AnimatedActionBase
{
	override void OnExecute(ActionData action_data)
	{
		super.OnExecute(action_data);

		if (g_Game.IsServer() && !IsInherited(ActionContinuousBase))
			ZenDirtyActionRules.Process(this, action_data);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionZenDirtyWashSelfWell);
		actions.Insert(ActionZenDirtyWashSelfWater);
		actions.Insert(ActionZenDirtyWashSelfContainer);
	}
}
