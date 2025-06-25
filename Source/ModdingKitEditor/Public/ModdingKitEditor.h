// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FModCreator;

class FModdingKitEditor: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	void AddModManagerButton();

	TSharedRef<SWidget> GetModManagerDropdown();

private:
	FDelegateHandle OnToolsMenuStartupHandle;

	TSharedPtr<FModCreator> ModCreator;

};