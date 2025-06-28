// Fill out your copyright notice in the Description page of Project Settings.


#include "ModPackager.h"
#include "ModPackagerWizard.h"

#define LOCTEXT_NAMESPACE "ModPackager"

const FName FModPackager::ModPackagerName = "ModPackager";

FModPackager::FModPackager()
{
	RegisterTabSpawner();
}

FModPackager::~FModPackager()
{
	UnregisterTabSpawner();
}

void FModPackager::OpenNewPackageWindow() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(ModPackagerName);
}

void FModPackager::RegisterTabSpawner()
{
	FTabSpawnerEntry& Spawner = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
	ModPackagerName, FOnSpawnTab::CreateRaw(this, &FModPackager::SpawnDockTab));

	// Set default size for this tab
	FVector2D DefaultSize = FVector2D(600.0f, 800.0f);
	FTabManager::RegisterDefaultTabWindowSize(ModPackagerName, DefaultSize);

	Spawner.SetDisplayName(LOCTEXT("PackageModTabHeader", "Mod Packager"));
	Spawner.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FModPackager::UnregisterTabSpawner()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ModPackagerName);
}

TSharedRef<SDockTab> FModPackager::SpawnDockTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SModPackagerWindow) // <-- your Slate widget goes here
		];
}

#undef LOCTEXT_NAMESPACE
