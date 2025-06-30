// Fill out your copyright notice in the Description page of Project Settings.


#include "ModCookingUtils.h"
#include "IUATHelperModule.h"
#include "Interfaces/IPluginManager.h"

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
	return "";
}

FString FModCookingUtils::MakeUATParams_BaseGame(const FString& UProjectFile)
{
	return "";
}

FString FModCookingUtils::MakeUATParams_DLC(const FString& DLCName)
{
	return "";
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

#undef LOCTEXT_NAMESPACE
