class ActionZenDirtyWashSelfWellCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float washTime = 0.1;

		if (m_ActionData && m_ActionData.m_Player)
		{
			ZenDirtyConfig config = GetZenDirtyConfig();

			if (config)
			{
				float currentDirtiness = Math.Clamp(m_ActionData.m_Player.ZenDirty_GetDirtiness(), 0.0, ZEN_DIRTY_MAX);

				washTime = config.WashWellActionSeconds * (currentDirtiness / ZEN_DIRTY_MAX);
				washTime = Math.Max(washTime, 0.1);
			}
		}

		m_ActionData.m_ActionComponent = new CAContinuousTime(washTime);
	}
}

class ActionZenDirtyWashSelfWell : ActionContinuousBase
{
	void ActionZenDirtyWashSelfWell()
	{
		m_CallbackClass = ActionZenDirtyWashSelfWellCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSWELL;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_ZENDIRTY_ACTION_WASH_SELF";
	}

	override ActionData CreateActionData()
	{
		ZenDirtyWashActionData data = new ZenDirtyWashActionData;
		return data;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !target)
			return false;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled || !config.WashingAtWellsEnabled)
			return false;

		if (!player.ZenDirty_IsDirty())
			return false;

		if (player.GetItemInHands())
			return false;

		Object targetObject = target.GetObject();

		if (!targetObject)
			return false;

		return (targetObject.GetWaterSourceObjectType() != EWaterSourceObjectType.NONE || targetObject.IsWell()) && player.ZenDirty_CanWash(ZEN_DIRTY_SRC_WELL);
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		if (!action_data || !action_data.m_Player)
			return;

		ZenDirtyWashActionData washActionData = ZenDirtyWashActionData.Cast(action_data);

		if (!washActionData)
			return;

		float currentDirtiness = action_data.m_Player.ZenDirty_GetDirtiness();

		washActionData.m_ZenDirtyStartDirtiness = currentDirtiness;
		washActionData.m_ZenDirtyMaximumCleanAmount = currentDirtiness;
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

		if (!action_data || !action_data.m_Player)
			return;

		if (progress <= 0.0)
			return;

		ZenDirtyWashActionData washActionData = ZenDirtyWashActionData.Cast(action_data);

		if (!washActionData)
			return;

		float dirtinessCleaned = washActionData.m_ZenDirtyMaximumCleanAmount * progress;
		dirtinessCleaned = Math.Min(dirtinessCleaned, action_data.m_Player.ZenDirty_GetDirtiness());

		if (dirtinessCleaned <= 0.0)
			return;

		action_data.m_Player.ZenDirty_CleanAmount(dirtinessCleaned, true);

		if (completed)
			action_data.m_Player.ZenDirty_ClearBloodyHands();
	}
}