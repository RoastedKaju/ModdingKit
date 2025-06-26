// Fill out your copyright notice in the Description page of Project Settings.


#include "ModCreatorWizard.h"

#include "Interfaces/IPluginManager.h"
#include "FindDirectoriesVisitor.h"
#include "Features/IPluginsEditorFeature.h"

#define LOCTEXT_NAMESPACE "ModCreatorWizard"

FModCreatorWizard::FModCreatorWizard()
{
	PluginBaseDirectory = IPluginManager::Get().FindPlugin(TEXT("ModdingKit"))->GetBaseDir();
	TArray<FString> FoundDirs;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString TemplateBaseDir = PluginBaseDirectory / TEXT("Templates");
	FFindDirectoriesVisitor FindDirectoriesVisitor(PlatformFile, FoundDirs);

	// Find the content only template that ships with the plugin
	// Download the Robo Recall Mod Kit and check the Plugins/OdinEditor code for how to build and use your own templates for  your game content

	// The base template that will be included with all created mods.
	FPluginTemplateDescription* BackingTemplateDesc = new FPluginTemplateDescription(
		FText::FromString("Default Template"), FText::FromString("Empty Content Package Template."), TEXT("BaseTemplate"), true, EHostType::Runtime);
	BackingTemplateDesc->SortPriority = 100;
	BackingTemplateDesc->bCanBePlacedInEngine = false;
	BackingTemplate = MakeShareable(BackingTemplateDesc);
	BackingTemplatePath = PluginBaseDirectory / TEXT("Templates") / BackingTemplate->OnDiskPath;
	TemplateDefinitions.Add(BackingTemplate.ToSharedRef());

	SelectedTemplates.Empty();
	SelectedTemplates.Add(BackingTemplate);

	// Find all additional mod templates and add them to the list of available selections
	FindTemplates();
}

void FModCreatorWizard::FindTemplates()
{
	PluginBaseDirectory = IPluginManager::Get().FindPlugin(TEXT("ModdingKit"))->GetBaseDir();
	TArray<FString> FoundDirs;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString TemplatesBaseDir = PluginBaseDirectory / TEXT("Templates");
	FFindDirectoriesVisitor FindDirectoriesVisitor(PlatformFile, FoundDirs);

	PlatformFile.IterateDirectory(*TemplatesBaseDir, FindDirectoriesVisitor);

	for (FString TemplateDirPath : FoundDirs)
	{
		FString TemplateDir = FPaths::GetCleanFilename(TemplateDirPath);

		// exclude template directories starting with two underscores (we consider those "disabled") and the base template, as we already added that one
		if ((!TemplateDir.StartsWith("__")) && (!TemplateDir.Equals("BaseTemplate")))
		{
			TArray<FString> TemplateDetails;
			FString TemplateName = FName::NameToDisplayString(TemplateDir, false);
			FString TemplateDescription = TEXT("");
			int32 TemplateSortPriority = 100;
			TSharedPtr<FPluginTemplateDescription> TemplateDescShrPtr;
			UE_LOG(LogTemp, Log, TEXT("Template details file path : %s"), *(TemplatesBaseDir / TemplateDir));
			FString TemplateDetailsFilePath = FString::Printf(TEXT("%s/TemplateInfo.txt"), *(TemplatesBaseDir / TemplateDir));

			// see if we have a .TXT file supplying us with better details for the template
			TemplateDetails.Empty();
			FFileHelper::LoadFileToStringArray(TemplateDetails, *TemplateDetailsFilePath);
			if (TemplateDetails.Num() >= 1)
			{
				TemplateName = TemplateDetails[0];
			}
			if (TemplateDetails.Num() >= 2)
			{
				TemplateDescription = TemplateDetails[1];
			}
			if (TemplateDetails.Num() >= 3)
			{
				TemplateSortPriority = FCString::Atoi(*TemplateDetails[2]);
			}

			FPluginTemplateDescription* TemplateDesc = new FPluginTemplateDescription(
				FText::FromString(TemplateName),
				FText::FromString(TemplateDescription),
				TemplateDir,
				true,
				EHostType::Runtime);
			TemplateDesc->SortPriority = TemplateSortPriority; // this seems to do nothing?!
			TemplateDesc->bCanBePlacedInEngine = false;
			TemplateDescShrPtr = MakeShareable(TemplateDesc);

			TemplateDefinitions.Add(TemplateDescShrPtr.ToSharedRef());
		}
	}
}

const TArray<TSharedRef<FPluginTemplateDescription>>& FModCreatorWizard::GetTemplatesSource() const
{
	return TemplateDefinitions;
}

void FModCreatorWizard::OnTemplateSelectionChanged(TSharedPtr<FPluginTemplateDescription> InSelectedItem,
                                                      ESelectInfo::Type SelectInfo)
{
	SelectedTemplates.Empty();
	SelectedTemplates.Add(InSelectedItem);
}

TSharedPtr<FPluginTemplateDescription> FModCreatorWizard::GetSelectedTemplate() const
{
	if (SelectedTemplates.Num() > 0)
	{
		return SelectedTemplates[0];
	}

	return BackingTemplate;
}

void FModCreatorWizard::ClearTemplateSelection()
{
	SelectedTemplates.Empty();
}

bool FModCreatorWizard::CanShowOnStartup() const
{
	return true;
}

bool FModCreatorWizard::HasValidTemplateSelection() const
{
	// A mod should be created even if no templates are selected.
	return true;
}

bool FModCreatorWizard::HasModules() const
{
	bool bHasModules = false;

	for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
	{
		if (FPaths::DirectoryExists(PluginBaseDirectory / TEXT("Templates") / Template->OnDiskPath / TEXT("Source")))
		{
			bHasModules = true;
			break;
		}
	}

	return bHasModules;
}

bool FModCreatorWizard::IsMod() const
{
	return true;
}

void FModCreatorWizard::OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState)
{
}

ECheckBoxState FModCreatorWizard::GetShowOnStartupCheckBoxState() const
{
	return ECheckBoxState();
}

FText FModCreatorWizard::GetInstructions() const
{
	return LOCTEXT("CreateNewModPanel", "Pick a template below and fill out the details.");
}

TSharedPtr<class SWidget> FModCreatorWizard::GetCustomHeaderWidget()
{
	if (!CustomHeaderWidget.IsValid())
	{
		FString IconPath;
		GetPluginIconPath(IconPath);

		const FName BrushName(*IconPath);
		const FIntPoint Size = FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(BrushName);
		if ((Size.X > 0) && (Size.Y > 0))
		{
			IconBrush = MakeShareable(new FSlateDynamicImageBrush(BrushName, FVector2D(Size.X, Size.Y)));
		}

		CustomHeaderWidget = SNew(SHorizontalBox)
			// Header image
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.0f)
			[
				SNew(SBox)
				.WidthOverride(80.0f)
				.HeightOverride(80.0f)
				[
					SNew(SImage)
					.Image(IconBrush.IsValid() ? IconBrush.Get() : nullptr)
				]
			];
	}

	return CustomHeaderWidget;
}

bool FModCreatorWizard::GetPluginIconPath(FString& OutIconPath) const
{
	OutIconPath = PluginBaseDirectory / TEXT("Resources/Icon128_ModCreator.png");
	return false;
}

bool FModCreatorWizard::GetTemplateIconPath(TSharedRef<FPluginTemplateDescription> InTemplate,
                                               FString& OutIconPath) const
{
	OutIconPath = PluginBaseDirectory / TEXT("Templates") / InTemplate->OnDiskPath / TEXT("Resources/Icon128.png");
	return false;
}

FString FModCreatorWizard::GetPluginFolderPath() const
{
	return BackingTemplatePath;
}

EHostType::Type FModCreatorWizard::GetPluginModuleDescriptor() const
{
	return BackingTemplate->ModuleDescriptorType;
}

ELoadingPhase::Type FModCreatorWizard::GetPluginLoadingPhase() const
{
	return BackingTemplate->LoadingPhase;
}

TArray<FString> FModCreatorWizard::GetFoldersForSelection() const
{
	TArray<FString> SelectedFolders;
	SelectedFolders.Add(BackingTemplatePath); // This will always be a part of the mod plugin

	for (TSharedPtr<FPluginTemplateDescription> Template : SelectedTemplates)
	{
		SelectedFolders.AddUnique(PluginBaseDirectory / TEXT("Templates") / Template->OnDiskPath);
	}

	return SelectedFolders;
}

void FModCreatorWizard::PluginCreated(const FString& PluginName, bool bWasSuccessful) const
{
	// Override Category to Mods
	if (bWasSuccessful)
	{
		TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName);
		if (Plugin != nullptr)
		{
			FPluginDescriptor Desc = Plugin->GetDescriptor();
			Desc.Category = "Game Mods";
			FText UpdateFailureText;
			Plugin->UpdateDescriptor(Desc, UpdateFailureText);
		}
	}
}

#undef LOCTEXT_NAMESPACE
