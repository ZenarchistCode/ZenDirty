#ifdef TerjeMedicine
modded class TerjePlayerModifierBase
{
	override float GetPlayerImmunity(PlayerBase player)
	{
		float immunity = super.GetPlayerImmunity(player);

		if (!player)
			return immunity;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled)
			return immunity;

		if (!player.ZenDirty_IsFullDirty())
			return immunity;

		return Math.Clamp(immunity * config.FullDirtyImmunityMultiplier, 0.0, 1.0);
	}
}
#endif
