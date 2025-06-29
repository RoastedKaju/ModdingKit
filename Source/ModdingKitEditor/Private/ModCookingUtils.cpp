// Fill out your copyright notice in the Description page of Project Settings.


#include "ModCookingUtils.h"

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
		
	}

	return Promise->GetFuture();
}

TFuture<bool> FModCookingUtils::CookMod(TSharedRef<IPlugin> Plugin, const FString& OutputDirectory,
	const FString& UProjectFile, const FName& PlatformNameIni)
{
	TSharedPtr<TPromise<bool>> Promise = MakeShared<TPromise<bool>>();

	return Promise->GetFuture();
}

void FModCookingUtils::OpenDialogBox()
{
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Success"));
}
