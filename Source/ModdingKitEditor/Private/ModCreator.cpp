// Fill out your copyright notice in the Description page of Project Settings.

#include "ModCreator.h"
#include "IPluginBrowser.h"
#include "ModCreatorWizard.h"

#define LOCTEXT_NAMESPACE "ModCreator"

const FName FModCreator::ModCreatorName = "ModCreator";

FModCreator::FModCreator()
{
	RegisterTabSpawner();
}

FModCreator::~FModCreator()
{
	UnregisterTabSpawner();
}

void FModCreator::OpenNewPluginWizard(bool bSuppressErrors) const
{
	if (IPluginBrowser::IsAvailable())
	{
		FGlobalTabmanager::Get()->TryInvokeTab(ModCreatorName);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("PluginBrowserDisabled",
		                                              "Creating a game mod requires the use of the Plugin Browser, but it is currently disabled."));
	}
}

void FModCreator::RegisterTabSpawner()
{
	FTabSpawnerEntry& Spawner = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ModCreatorName, FOnSpawnTab::CreateRaw(this, &FModCreator::SpawnPluginTab));

	// Set default size for this tab
	FVector2D DefaultSize = FVector2D(900.0f, 800.0f);
	FTabManager::RegisterDefaultTabWindowSize(ModCreatorName, DefaultSize);

	Spawner.SetDisplayName(LOCTEXT("NewModTabHeader", "Mod Creator"));
	Spawner.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FModCreator::UnregisterTabSpawner()
{
	FGlobalTabmanager::Get()->UnregisterTabSpawner(ModCreatorName);
}

TSharedRef<SDockTab> FModCreator::SpawnPluginTab(const FSpawnTabArgs& Args)
{
	check(IPluginBrowser::IsAvailable());
	return IPluginBrowser::Get().SpawnPluginCreatorTab(Args, MakeShared<FModCreatorWizard>());
}

#undef LOCTEXT_NAMESPACE
