#ifdef JM_COT
modded class JMPlayerForm
{
	UIActionSlider m_ZenDirty;
	bool m_ZenDirtyUpdated;

	override private Widget InitActionWidgetsStats(Widget actionsParent)
	{
		Widget parent = super.InitActionWidgetsStats(actionsParent);

		if (!m_ZenDirty && m_Health)
		{
			Widget parentWidgie = m_Health.GetLayoutRoot().GetParent();

			m_ZenDirty = UIActionManager.CreateSlider(parentWidgie, "ZenDirty:", 0, ZEN_DIRTY_MAX, this, "Click_SetZenDirty");
			m_ZenDirty.SetSliderWidth(0.5);
		}

		return parent;
	}

	override void RefreshStats(bool force = false)
	{
		super.RefreshStats(force);

		if (!m_SelectedInstance)
			return;

		if (g_Game.IsClient() && m_SelectedInstance.GetDataLastUpdatedTime() < m_LastChangeTime)
			return;

		if (force)
		{
			m_ZenDirtyUpdated = false;
		}

		if (!m_ZenDirty)
			return;

		if (!m_ZenDirtyUpdated)
		{
			m_ZenDirty.SetCurrent(m_SelectedInstance.GetZenDirty());
		}

		ZenDirty_UpdateSliderColor();
	}

	void Click_SetZenDirty(UIEvent eid, UIActionBase action)
	{
		if (eid != UIEvent.CHANGE)
			return;

		UpdateLastChangeTime();

		ZenDirty_UpdateSliderColor();

		m_ZenDirtyUpdated = true;
	}

	protected void ZenDirty_UpdateSliderColor()
	{
		if (!m_ZenDirty)
			return;

		float dirtiness = m_ZenDirty.GetCurrent();

		if (dirtiness <= 25.0)
		{
			m_ZenDirty.SetColor(Colors.COLOR_PRISTINE);
			m_ZenDirty.SetAlpha(1.0);
		}
		else if (dirtiness <= 50.0)
		{
			m_ZenDirty.SetColor(ARGB(255, 220, 220, 220));
		}
		else if (dirtiness <= 75.0)
		{
			m_ZenDirty.SetColor(ARGB(255, 220, 220, 0));
		}
		else
		{
			m_ZenDirty.SetColor(ARGB(255, 220, 0, 0));
		}
	}

	override void Click_ApplyStats(UIEvent eid, UIActionBase action)
	{
		super.Click_ApplyStats(eid, action);

		if (eid != UIEvent.CLICK)
			return;

		if (!m_ZenDirtyUpdated)
			return;

		m_ZenDirtyUpdated = false;

		if (m_ZenDirty)
		{
			m_Module.SetZenDirty(m_ZenDirty.GetCurrent(), JM_GetSelected().GetPlayersOrSelf());
		}
	}
}
#endif