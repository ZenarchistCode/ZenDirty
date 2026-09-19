class CfgPatches
{
	class ZenDirty
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"JM_CF_Scripts",
			"ZenModCore"
		};
	};
};

class CfgMods
{
	class ZenDirty
	{
		dir = "ZenDirty";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "ZenDirty";
		credits = "";
		author = "Zenarchist";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		storageVersion = 1;
		dependencies[] =
		{
			"Game",
			"World",
			"Mission"
		};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] =
				{
					"ZenDirty/scripts/3_Game"
				};
			};

			class worldScriptModule
			{
				value = "";
				files[] =
				{
					"ZenDirty/scripts/4_World"
				};
			};

			class missionScriptModule
			{
				value = "";
				files[] =
				{
					"ZenDirty/scripts/5_Mission"
				};
			};
		};
	};
};
