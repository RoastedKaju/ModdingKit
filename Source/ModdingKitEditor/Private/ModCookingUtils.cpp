// Fill out your copyright notice in the Description page of Project Settings.


#include "ModCookingUtils.h"
#include "IUATHelperModule.h"
#include "Interfaces/IPluginManager.h"
#include "Settings/ProjectPackagingSettings.h"
#include "IDesktopPlatform.h"
#define UE_VERSION_NEWER_THAN(X, Y) (ENGINE_MAJOR_VERSION > X) || (ENGINE_MAJOR_VERSION == X && ENGINE_MINOR_VERSION > Y)
#if UE_VERSION_NEWER_THAN(5, 2)
	#include "Settings/PlatformsMenuSettings.h"
#endif

#define LOCTEXT_NAMESPACE "ModCookingUtils"

static const FString CookFlavor = "";
static const FString ConfigurationName = "Shipping";
static const bool bCompress = false;
static const FString ReleaseVersionName = "BaseGameRelease";

TFuture<bool> FModCookingUtils::CookProject(const FString& OutputDirectory, const FString& UProjectFile,
                                            const FName& PlatformNameIni)
{
	TSharedPtr<TPromise<bool>> Promise = MakeShared<TPromise<bool>>();

	if (FPaths::DirectoryExists(FPaths::ProjectDir() / "Releases" / ReleaseVersionName / PlatformNameIni.ToString()))
	{
		UE_LOG(LogTemp, Log, TEXT("Existing release found, skipping base game cook"));
		Promise->SetValue(true);
	}
	else
	{
		const FString StagingDirectory = FPaths::Combine(OutputDirectory, TEXT("__TMP_STAGING__"));
		const FString CookProjectCommand = MakeUATCommand(UProjectFile, PlatformNameIni, StagingDirectory) +
			MakeUATParams_BaseGame(UProjectFile);

		IUATHelperModule::Get().CreateUatTask(
			CookProjectCommand, FText::FromString(PlatformNameIni.ToString()),
			FText::FromString("Packaging Base Game..."), FText::FromString("Packaging Base Game..."),
			FAppStyle::Get().GetBrush("Icons.PackageProject"), nullptr,
			[Promise, StagingDirectory](FString TaskResult, double TimeSec)
			{
				AsyncTask(ENamedThreads::GameThread, [Promise, TaskResult, StagingDirectory]()
				{
					// Clean up the staging directory
					IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
					PlatformFile.DeleteDirectoryRecursively(*StagingDirectory);

					if (TaskResult == "Completed")
					{
						UE_LOG(LogTemp, Log, TEXT("Successfully cooked project for UGC"));
						Promise->SetValue(true);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to cook project for UGC: %s"), *TaskResult);
						Promise->SetValue(false);
					}
				});
			});
	}

	return Promise->GetFuture();
}

TFuture<bool> FModCookingUtils::CookMod(TSharedRef<IPlugin> Plugin, const FString& OutputDirectory,
                                        const FString& UProjectFile, const FName& PlatformNameIni)
{
	bool bWasUsingSharedShaderCode = IsShareMaterialShaderCodeEnabled();
	SetShareMaterialShaderCodeEnabled(false);

	TSharedPtr<TPromise<bool>> Promise = MakeShared<TPromise<bool>>();

	const FString CookPluginCommand = MakeUATCommand(UProjectFile, PlatformNameIni, OutputDirectory) + MakeUATParams_DLC(*Plugin->GetName());

	FText PackagingText = FText::Format(LOCTEXT("ModCookingUtils", "Packaging {0}"), FText::FromString(Plugin->GetName()));

	IUATHelperModule::Get().CreateUatTask(
	CookPluginCommand, FText::FromString(PlatformNameIni.ToString()), PackagingText, PackagingText,
	FAppStyle::Get().GetBrush("Icons.PackageProject"), nullptr,
	[Promise, Plugin, bWasUsingSharedShaderCode](FString TaskResult, double TimeSec) {
		AsyncTask(ENamedThreads::GameThread, [Promise, Plugin, TaskResult, bWasUsingSharedShaderCode]() {
			// Reset our share shader code value to what it was prior to cook
			SetShareMaterialShaderCodeEnabled(bWasUsingSharedShaderCode);

			if (TaskResult == "Completed")
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully packaged plugin '%s' as UGC"), *Plugin->GetName());
				Promise->SetValue(true);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to package plugin '%s' as UGC: %s"), *Plugin->GetName(),
					   *TaskResult);
				Promise->SetValue(false);
			}
		});
	});

	return Promise->GetFuture();
}

void FModCookingUtils::OpenDialogBox()
{
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Success"));
}

FString FModCookingUtils::MakeUATCommand(const FString& UProjectFile, const FName& PlatformNameIni,
	const FString& StageDirectory)
{
	const FDataDrivenPlatformInfo& DDPI = FDataDrivenPlatformInfoRegistry::GetPlatformInfo(PlatformNameIni);
	FString UBTPlatformString = DDPI.UBTPlatformString;

	FString CommandLine = FString::Printf(TEXT("BuildCookRun -project=\"%s\" -noP4"), *UProjectFile);

	CommandLine += FString::Printf(TEXT(" -clientconfig=%s -serverconfig=%s"), *ConfigurationName, *ConfigurationName);

	// UAT should be compiled already
	CommandLine += " -nocompile -nocompileeditor";

	CommandLine += FApp::IsEngineInstalled() ? TEXT(" -installed") : TEXT("");

	CommandLine += " -utf8output";

	CommandLine += " -platform=" + UBTPlatformString;

	if (CookFlavor.Len() > 0)
	{
		CommandLine += " -cookflavor=" + CookFlavor;
	}

	CommandLine += " -build -cook -CookCultures=en -unversionedcookedcontent -pak";

	if (bCompress)
	{
		CommandLine += " -compressed";
	}

	// Taken from TurnkeySupportModule.cpp
	{
		UProjectPackagingSettings* AllPlatformPackagingSettings = GetMutableDefault<UProjectPackagingSettings>();

#if UE_VERSION_NEWER_THAN(5, 2)
		UPlatformsMenuSettings* PlatformsSettings = GetMutableDefault<UPlatformsMenuSettings>();
#endif

		bool bIsProjectBuildTarget = false;
		const FTargetInfo* BuildTargetInfo =
#if UE_VERSION_NEWER_THAN(5, 2)
			PlatformsSettings->
#else
			AllPlatformPackagingSettings->
#endif
			GetBuildTargetInfoForPlatform(PlatformNameIni, bIsProjectBuildTarget);

		// Only add the -Target=... argument for code projects. Content projects will return
		// UnrealGame/UnrealClient/UnrealServer here, but may need a temporary target generated to enable/disable
		// plugins. Specifying -Target in these cases will cause packaging to fail, since it'll have a different name.
		if (BuildTargetInfo && bIsProjectBuildTarget)
		{
			CommandLine += FString::Printf(TEXT(" -target=%s"), *BuildTargetInfo->Name);
		}

		// optional settings
		if (AllPlatformPackagingSettings->bSkipEditorContent)
		{
			CommandLine += TEXT(" -SkipCookingEditorContent");
		}

		if (AllPlatformPackagingSettings->FullRebuild)
		{
			CommandLine += TEXT(" -clean");
		}
	}

	CommandLine += " -stage";

	CommandLine += FString::Printf(TEXT(" -stagingdirectory=\"%s\""), *StageDirectory);

	return CommandLine;
}

FString FModCookingUtils::MakeUATParams_BaseGame(const FString& UProjectFile)
{
	FString OutParams = FString::Printf(TEXT(" -package -createreleaseversion=\"%s\""), *ReleaseVersionName);

	TArray<FString> Result;
	TArray<FString> ProjectMapNames;

	const FString WildCard = FString::Printf(TEXT("*%s"), *FPackageName::GetMapPackageExtension());

	// Scan all Content folder, because not all projects follow Content/Maps convention
	IFileManager::Get().FindFilesRecursive(
		ProjectMapNames, *FPaths::Combine(FPaths::GetPath(UProjectFile), TEXT("Content")), *WildCard, true, false);

	for (int32 i = 0; i < ProjectMapNames.Num(); i++)
	{
		Result.Add(FPaths::GetBaseFilename(ProjectMapNames[i]));
	}

	Result.Sort();

	if (Result.Num() > 0)
	{
		// Our goal is to only have assets from inside the actual plugin content folder.
		// In order for Unreal to only put these assets inside the pak and not assets from /Game we have to specify all
		// maps from /Game to the command line for UAT. Format: -map=Value1+Value2+Value3

		OutParams += " -map=";

		for (int32 i = 0; i < Result.Num(); i++)
		{
			OutParams += Result[i];

			if (i + 1 < Result.Num())
			{
				OutParams += "+";
			}
		}
	}

	return OutParams;
}

FString FModCookingUtils::MakeUATParams_DLC(const FString& DLCName)
{
	FString CommandLine = FString::Printf(TEXT(" -basedonreleaseversion=\"%s\""), *ReleaseVersionName);

	CommandLine += " -stagebasereleasepaks";

	CommandLine += FString::Printf(TEXT(" -DLCName=\"%s\""), *DLCName);

	return CommandLine;
}

bool FModCookingUtils::IsShareMaterialShaderCodeEnabled()
{
	FString IniFile = FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("DefaultGame.ini"));

	bool bEnabled = false;
	GConfig->GetBool(TEXT("/Script/UnrealEd.ProjectPackagingSettings"), TEXT("bShareMaterialShaderCode"), bEnabled,
					 IniFile);
	return bEnabled;
}

void FModCookingUtils::SetShareMaterialShaderCodeEnabled(bool bEnabled)
{
	FString IniFile = FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("DefaultGame.ini"));

	// Set the value and flush to ensure it's written to disk and not just in memory
	GConfig->SetBool(TEXT("/Script/UnrealEd.ProjectPackagingSettings"), TEXT("bShareMaterialShaderCode"), bEnabled,
					 IniFile);
	GConfig->Flush(false, IniFile);
}

void FModCookingUtils::FindAvailablePlugins(TArray<TSharedRef<IPlugin>>& OutAvailableGameMods)
{
	OutAvailableGameMods.Empty();

	// Find available game mods from the list of discovered plugins
	for (TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetDiscoveredPlugins())
	{
		// All game project plugins that are marked as mods are valid
		if (Plugin->GetLoadedFrom() == EPluginLoadedFrom::Project && Plugin->GetType() == EPluginType::Mod)
		{
			OutAvailableGameMods.AddUnique(Plugin);
		}
	}
}

FString FModCookingUtils::GetProjectPath()
{
	if (FPaths::IsProjectFilePathSet())
	{
		return FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
	}
	if (FApp::HasProjectName())
	{
		FString ProjectPath = FPaths::ProjectDir() / FApp::GetProjectName() + TEXT(".uproject");
		if (FPaths::FileExists(ProjectPath))
		{
			return ProjectPath;
		}
		ProjectPath = FPaths::RootDir() / FApp::GetProjectName() / FApp::GetProjectName() + TEXT(".uproject");
		if (FPaths::FileExists(ProjectPath))
		{
			return ProjectPath;
		}
	}
	return FString();
}

#undef LOCTEXT_NAMESPACE
