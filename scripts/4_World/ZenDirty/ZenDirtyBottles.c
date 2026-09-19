modded class Bottle_Base
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionZenDirtyWashSelfContainer);
	}
}

modded class Well
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionZenDirtyWashSelfWell);
	}
}