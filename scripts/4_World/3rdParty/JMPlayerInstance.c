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

	override void OnSendStatus(ParamsWriteContext ctx)
	{
		super.OnSendStatus(ctx);

		ctx.Write(m_ZenDirty);
	}

	override bool OnReceiveStatus(ParamsReadContext ctx)
	{
		if (!super.OnReceiveStatus(ctx))
			return false;

		return ctx.Read(m_ZenDirty);
	}
}
#endif