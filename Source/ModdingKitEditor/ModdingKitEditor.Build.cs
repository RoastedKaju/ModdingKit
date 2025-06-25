using UnrealBuildTool;

public class ModdingKitEditor : ModuleRules
{
	public ModdingKitEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"Slate",
					"SlateCore",
					"UnrealEd",
					"ToolMenus", 
					"PluginBrowser"
				}
			);
	}
}