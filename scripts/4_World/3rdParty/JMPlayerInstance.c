#ifdef JM_COT
modded class JMPlayerInstance
{
	protected float m_ZenDirty;

	override void Update()
	{
		if (g_Game.IsServer() && (g_Game.GetTime() - m_DataLastUpdated) >= 100)
		{
			if (!g_Game.IsMultiplayer())
			{
				Class.CastTo(PlayerObject, g_Game.GetPlayer());
			}

			if (PlayerObject)
			{
				m_ZenDirty = PlayerObject.ZenDirty_GetDirtiness();
			}
		}

		super.Update();
	}

	float GetZenDirty()
	{
		return m_ZenDirty;
	}

	override void OnSendHealth(ParamsWriteContext ctx)
	{
		super.OnSendHealth(ctx);

		ctx.Write(m_ZenDirty);
	}

	override void OnRecieveHealth(ParamsReadContext ctx)
	{
		super.OnRecieveHealth(ctx);

		ctx.Read(m_ZenDirty);
	}
}
#endif