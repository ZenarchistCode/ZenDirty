modded class IngameHud
{
	protected Widget m_ZenDirtyBadgeRoot;
	protected ImageWidget m_ZenDirtyBadge;
	protected string m_ZenDirtyLoadedBadgeTexture;
	protected bool m_ZenDirtyBadgeVisible;
	protected float m_ZenDirtyBadgeUpdateAccumulator;

	override void Init(Widget hud_panel_widget)
	{
		super.Init(hud_panel_widget);
		ZenDirty_InitBadge();
	}

	protected void ZenDirty_InitBadge()
	{
		if (!m_Badges || m_ZenDirtyBadgeRoot)
			return;

		m_ZenDirtyBadgeRoot = g_Game.GetWorkspace().CreateWidgets(ZEN_DIRTY_BADGE_LAYOUT, m_Badges);
		if (!m_ZenDirtyBadgeRoot)
			return;

		m_ZenDirtyBadge = ImageWidget.Cast(m_ZenDirtyBadgeRoot);
		if (!m_ZenDirtyBadge)
			m_ZenDirtyBadge = ImageWidget.Cast(m_ZenDirtyBadgeRoot.FindAnyWidget("ZenDirtyBadge"));

		if (!m_ZenDirtyBadge)
			return;

		m_ZenDirtyBadge.Show(false);
		ZenDirty_RefreshBadge(true);
	}

	override void DisplayBadge(int key, int value)
	{
		super.DisplayBadge(key, value);
		ZenDirty_UpdateBadgePanelVisibility();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		m_ZenDirtyBadgeUpdateAccumulator += timeslice;
		if (m_ZenDirtyBadgeUpdateAccumulator < 0.25)
			return;

		m_ZenDirtyBadgeUpdateAccumulator = 0.0;
		ZenDirty_RefreshBadge(false);
	}

	protected void ZenDirty_RefreshBadge(bool forceTextureReload)
	{
		if (!m_ZenDirtyBadge)
		{
			ZenDirty_InitBadge();
			return;
		}

		ZenDirtyConfig config = GetZenDirtyConfig();
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());

		bool shouldShow = false;
		string badgeTexture = ZEN_DIRTY_DEFAULT_BADGE_TEXTURE;

		if (config)
		{
			badgeTexture = config.DirtyBadgeTexture;

			if (config.Enabled && config.ShowDirtyBadge && player && player.ZenDirty_IsFullDirty())
				shouldShow = true;
		}

		if (badgeTexture == "")
			badgeTexture = ZEN_DIRTY_DEFAULT_BADGE_TEXTURE;

		if (forceTextureReload || badgeTexture != m_ZenDirtyLoadedBadgeTexture)
		{
			m_ZenDirtyBadge.LoadImageFile(0, badgeTexture);
			m_ZenDirtyBadge.SetImage(0);
			m_ZenDirtyBadge.SetColor(GetZenDirtyBadgeColor());
			m_ZenDirtyLoadedBadgeTexture = badgeTexture;
		}

		if (shouldShow != m_ZenDirtyBadgeVisible)
		{
			m_ZenDirtyBadgeVisible = shouldShow;
			m_ZenDirtyBadge.Show(shouldShow);

			if (m_Badges)
				m_Badges.Update();
		}

		ZenDirty_UpdateBadgePanelVisibility();
	}

	int GetZenDirtyBadgeColor()
	{
		return ARGB(255, 150, 95, 50); //ARGB(255, 125, 75, 35);
	}

	protected void ZenDirty_UpdateBadgePanelVisibility()
	{
		if (!m_HudVisibility)
			return;

		if (m_ZenDirtyBadgeVisible)
		{
			m_HudVisibility.SetContextFlag(EHudContextFlags.NO_BADGE, false);
			return;
		}

		bool vanillaBadgeVisible = false;

		if (m_BadgesWidgetDisplay)
		{
			for (int i = 0; i < m_BadgesWidgetDisplay.Count(); i++)
			{
				if (m_BadgesWidgetDisplay.GetElement(i) > 0)
				{
					vanillaBadgeVisible = true;
					break;
				}
			}
		}

		m_HudVisibility.SetContextFlag(EHudContextFlags.NO_BADGE, !vanillaBadgeVisible);
	}
}
