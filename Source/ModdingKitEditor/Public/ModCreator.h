// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FModCreator : public TSharedFromThis<FModCreator>
{
public:
	FModCreator();
	~FModCreator();

	void OpenNewPluginWizard(bool bSuppressErrors = false) const;

public:
	// The name to use when creating a new tab for tab spawner
	static const FName ModCreatorName;

private:
	void RegisterTabSpawner();
	void UnregisterTabSpawner();

	// Spawn the tab that holds the mod creator wizard
	TSharedRef<SDockTab> SpawnPluginTab(const FSpawnTabArgs& Args);
};
