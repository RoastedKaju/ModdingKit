// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/Future.h"

class IPlugin;

/**
 * 
 */
class FModCookingUtils final
{
public:
	/**
	 * @brief Cooks the project before attempting to cook the mod plugin.
	 */
	static TFuture<bool> CookProject(const FString& OutputDirectory, const FString& UProjectFile, const FName& PlatformNameIni);

	/**
	 * @brief Cooks the plugin as DLC
	 */
	static TFuture<bool> CookMod(TSharedRef<IPlugin> Plugin, const FString& OutputDirectory, const FString& UProjectFile, const FName& PlatformNameIni);

	static void OpenDialogBox();

protected:
	static FString MakeUATCommand(const FString& UProjectFile, const FName& PlatformNameIni,
							  const FString& StageDirectory);
	static FString MakeUATParams_BaseGame(const FString& UProjectFile);
	static FString MakeUATParams_DLC(const FString& DLCName);

	static bool IsShareMaterialShaderCodeEnabled();
	static void SetShareMaterialShaderCodeEnabled(bool bEnabled);
	static void FindAvailablePlugins(TArray<TSharedRef<IPlugin>>& OutAvailableGameMods);
};
