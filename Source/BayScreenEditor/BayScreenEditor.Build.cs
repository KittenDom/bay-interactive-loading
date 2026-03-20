using UnrealBuildTool;

public class BayScreenEditor : ModuleRules
{
	public BayScreenEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"PropertyEditor",
				"UnrealEd",

				"BayScreenRuntime",
				"BayScreenShared"
			}
		);
	}
}