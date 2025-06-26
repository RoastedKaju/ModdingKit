// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FModPackager : public TSharedFromThis<FModPackager>
{
public:
	FModPackager();
	~FModPackager();

	void OpenNewPackageWindow() const;

public:
	// The name to use when creating a new tab for tab spawner
	static const FName ModPackagerName;

private:
	void RegisterTabSpawner();
	void UnregisterTabSpawner();

	// Spawn the tab that holds the mod creator wizard
	TSharedRef<SDockTab> SpawnDockTab(const FSpawnTabArgs& Args);
};
