modded class MissionBase
{
	void MissionBase()
	{
		#ifdef JM_COT
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenDirtySetStatCOT", this, SingeplayerExecutionType.Server);
		#endif
	}

	#ifdef JM_COT
	void RPC_ReceiveZenDirtySetStatCOT(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		JMPlayerInstance instance;

		if (!GetPermissionsManager().HasPermission("Admin.Player.Set.ZenDirty", sender, instance))
			return;

		Param2<float, array<string>> data;

		if (!ctx.Read(data))
		{
			Error("[ZenDirty] RPC_ReceiveZenDirtySetStatCOT: sync data read error");
			return;
		}

		float dirtiness = Math.Clamp(data.param1, 0.0, ZEN_DIRTY_MAX);

		array<Man> players = new array<Man>();
		g_Game.GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase player;

			if (!Class.CastTo(player, man))
				continue;

			if (!player.GetIdentity())
				continue;

			if (data.param2.Find(player.GetIdentity().GetId()) == -1)
				continue;

			player.ZenDirty_SetDirtiness(dirtiness);
		}
	}
	#endif
}