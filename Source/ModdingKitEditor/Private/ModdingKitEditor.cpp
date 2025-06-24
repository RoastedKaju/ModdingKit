#include "ModdingKitEditor.h"
#include "ToolMenus.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "ModdingKitEditorModule"

IMPLEMENT_GAME_MODULE(FModdingKitEditor, ModdingKitEditor)

void FModdingKitEditor::StartupModule()
{
	IModuleInterface::StartupModule();

	UE_LOG(LogTemp, Log, TEXT("ModdingKitEditor started!"));

	if (FSlateApplication::IsInitialized())
	{
		UE_LOG(LogTemp, Log, TEXT("FSlateApplication is valid."));

		OnToolsMenuStartupHandle = UToolMenus::RegisterStartupCallback(
			FSimpleMulticastDelegate::FDelegate::CreateStatic(&AddModManagerButton));
	}
}

void FModdingKitEditor::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

void FModdingKitEditor::AddModManagerButton()
{
	UE_LOG(LogTemp, Log, TEXT("Adding mod manager button"));
	UToolMenu* ToolMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = ToolMenu->AddSection("ModManagerSection", TAttribute<FText>(),
	                                                 FToolMenuInsert("Play", EToolMenuInsertType::After));

	// Add combo button
	FToolMenuEntry ModManagerEntry = FToolMenuEntry::InitComboButton(
		"ModManagerDropdown",
		FUIAction(),
		FOnGetContent::CreateStatic(&GetModManagerDropdown),
		LOCTEXT("ModManager_Label", "Mod Manager"),
		LOCTEXT("ModManager_ToolTip", "Create or package mod"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "MainFrame.PackageProject"));

	ModManagerEntry.StyleNameOverride = "CalloutToolBar";
	Section.AddEntry(ModManagerEntry);
}

TSharedRef<SWidget> FModdingKitEditor::GetModManagerDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("CreateMod", "Create Mod"),
		LOCTEXT("CreateModTooltip", "Start creating a new mod"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([]()
		{
			UE_LOG(LogTemp, Log, TEXT("Create Mod selected"));
		}))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("PackageMod", "Package Mod"),
		LOCTEXT("PackageModTooltip", "Package the selected mod"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([]()
		{
			UE_LOG(LogTemp, Log, TEXT("Package Mod selected"));
		}))
	);

	return MenuBuilder.MakeWidget();
}


#undef LOCTEXT_NAMESPACE
