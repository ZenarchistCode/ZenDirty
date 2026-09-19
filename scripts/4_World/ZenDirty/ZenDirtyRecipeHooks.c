class ZenDirtyRecipeRules
{
	static float GetDirtiness(RecipeBase recipe)
	{
		if (!recipe)
			return 0.0;

		ZenDirtyConfig config = GetZenDirtyConfig();

		if (!config || !config.Enabled || !config.RecipeDirtiness)
			return 0.0;

		string recipeClass = recipe.ClassName();
		float dirtiness;

		// Exact configured class always wins.
		if (config.RecipeDirtiness.Find(recipeClass, dirtiness))
			return Math.Max(dirtiness, 0.0);

		/*
			Otherwise find the MOST SPECIFIC configured parent class.

			Example:
				PrepareCarp -> PrepareFish -> PrepareAnimal

			If both PrepareFish and PrepareAnimal are configured,
			PrepareFish must win regardless of which has the larger
			dirtiness value.
		*/
		typename bestConfiguredType;
		float inheritedDirtiness = 0.0;

		for (int i = 0; i < config.RecipeDirtiness.Count(); i++)
		{
			string configuredClass = config.RecipeDirtiness.GetKey(i);
			typename configuredType = configuredClass.ToType();

			if (!configuredType)
				continue;

			if (!recipe.IsInherited(configuredType))
				continue;

			/*
				If we haven't found anything yet, use this match.

				Otherwise only replace the current match if this configured
				type inherits from the previous match, meaning it is further
				down the inheritance tree and therefore more specific.
			*/
			if (!bestConfiguredType || configuredType.IsInherited(bestConfiguredType))
			{
				bestConfiguredType = configuredType;
				inheritedDirtiness = config.RecipeDirtiness.GetElement(i);
			}
		}

		return Math.Max(inheritedDirtiness, 0.0);
	}
}

modded class RecipeBase
{
	override void PerformRecipe(ItemBase item1, ItemBase item2, PlayerBase player)
	{
		float dirtiness = 0.0;
		bool shouldAddDirtiness = false;

		if (g_Game.IsServer() && player)
		{
			dirtiness = ZenDirtyRecipeRules.GetDirtiness(this);

			if (dirtiness > 0.0 && item1 && item2)
				shouldAddDirtiness = CheckRecipe(item1, item2, player);
		}

		super.PerformRecipe(item1, item2, player);

		if (shouldAddDirtiness && player)
			player.ZenDirty_AddDirtiness(dirtiness);
	}
}
