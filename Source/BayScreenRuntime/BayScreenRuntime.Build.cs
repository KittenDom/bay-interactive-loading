using UnrealBuildTool;

public class BayScreenRuntime : ModuleRules
{
	public BayScreenRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"GameplayTags",
				"InputCore",
				"MoviePlayer",
				"Slate",
				"SlateCore",

				"BayScreenShared",
			}
		);
	}
}