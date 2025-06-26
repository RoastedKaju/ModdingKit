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
					"InputCore",
					"Slate",
					"SlateCore",
					"UnrealEd",
					"EditorStyle",
					"ToolMenus", 
					"PluginBrowser",
					"Projects",
					"Json",
					"JsonSerialization"
				}
			);
	}
}