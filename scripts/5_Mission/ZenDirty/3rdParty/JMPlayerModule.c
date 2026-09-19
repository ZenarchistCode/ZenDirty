#ifdef JM_COT
modded class JMPlayerModule
{
	void JMPlayerModule()
	{
		GetPermissionsManager().RegisterPermission("Admin.Player.Set.ZenDirty");
	}

	void SetZenDirty(float dirtiness, array<string> guids)
	{
		if (IsMissionHost())
			return;

		GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenDirtySetStatCOT", new Param2<float, array<string>>(dirtiness, guids), true, NULL);
	}
}
#endif