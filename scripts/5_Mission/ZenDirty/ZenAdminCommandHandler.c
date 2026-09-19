modded class ZenAdminCommandHandler
{
	override bool HandleAdminCommand(PlayerBase player, string uid, string cmd, array<string> params)
	{
		if (super.HandleAdminCommand(player, uid, cmd, params))
			return true;

		PlayerBase targetPlayer;
		float dirtiness;
		
		if (cmd == "togglezendirty")
		{
			targetPlayer = player;

			if (params.Count() > 0)
			{
				targetPlayer = ZenFunctions.GetPlayerByID(params.Get(0));

				if (!targetPlayer)
				{
					SendMsg(player, "Could not find online player with ID: " + params.Get(0));
					return true;
				}
			}

			float newDirtiness = 100.0;

			if (targetPlayer.ZenDirty_GetDirtiness() >= 100.0)
				newDirtiness = 0.0;

			targetPlayer.ZenDirty_SetDirtiness(newDirtiness);

			SendMsg(player, "Set " + targetPlayer.GetCachedName() + " dirtiness to " + newDirtiness.ToString() + "%");

			return true;
		}

		if (cmd == "setzendirty")
		{
			if (params.Count() < 1)
			{
				SendMsg(player, "Usage: !setzendirty <amount> <optionalPlayerID>");
				return true;
			}

			dirtiness = Math.Clamp(params.Get(0).ToFloat(), 0.0, 100.0);
			targetPlayer = player;

			if (params.Count() > 1)
			{
				targetPlayer = ZenFunctions.GetPlayerByID(params.Get(1));

				if (!targetPlayer)
				{
					SendMsg(player, "Could not find online player with ID: " + params.Get(1));
					return true;
				}
			}

			targetPlayer.ZenDirty_SetDirtiness(dirtiness);

			SendMsg(player, "Set " + targetPlayer.GetCachedName() + " dirtiness to " + dirtiness.ToString() + "%");

			return true;
		}

		if (cmd == "getzendirty")
		{
			targetPlayer = player;

			if (params.Count() > 0)
			{
				targetPlayer = ZenFunctions.GetPlayerByID(params.Get(0));

				if (!targetPlayer)
				{
					SendMsg(player, "Could not find online player with ID: " + params.Get(0));
					return true;
				}
			}

			dirtiness = targetPlayer.ZenDirty_GetDirtiness();

			SendMsg(player, targetPlayer.GetCachedName() + " dirtiness is currently " + dirtiness.ToString() + "%");

			return true;
		}

		return false;
	}
}
