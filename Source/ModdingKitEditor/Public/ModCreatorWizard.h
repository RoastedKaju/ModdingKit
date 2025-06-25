// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPluginWizardDefinition.h"

struct FSlateDynamicImageBrush;
class SWidget;

/**
 * 
 */
class FModCreatorWizard : public IPluginWizardDefinition
{
public:
	FModCreatorWizard();

	void FindTemplates();

	// Begin IPluginWizardDefinition interface
	virtual const TArray<TSharedRef<FPluginTemplateDescription>>& GetTemplatesSource() const override;
	virtual void OnTemplateSelectionChanged(TSharedPtr<FPluginTemplateDescription> InSelectedItem, ESelectInfo::Type SelectInfo) override;
	virtual bool HasValidTemplateSelection() const override;
	virtual TSharedPtr<FPluginTemplateDescription> GetSelectedTemplate() const override;
	virtual void ClearTemplateSelection() override;
	virtual bool CanShowOnStartup() const override;
	virtual bool HasModules() const override;
	virtual bool IsMod() const override;
	virtual void OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState) override;
	virtual ECheckBoxState GetShowOnStartupCheckBoxState() const override;
	virtual TSharedPtr<class SWidget> GetCustomHeaderWidget() override;
	virtual FText GetInstructions() const override;
	virtual bool GetPluginIconPath(FString& OutIconPath) const override;
	virtual EHostType::Type GetPluginModuleDescriptor() const override;
	virtual ELoadingPhase::Type GetPluginLoadingPhase() const override;
	virtual bool GetTemplateIconPath(TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const override;
	virtual FString GetPluginFolderPath() const override;
	virtual TArray<FString> GetFoldersForSelection() const override;
	virtual void PluginCreated(const FString& PluginName, bool bWasSuccessful) const override;
	// End IPluginWizardDefinition interface

private:
	TArray<TSharedRef<FPluginTemplateDescription>> TemplateDefinitions;
	TArray<TSharedPtr<FPluginTemplateDescription>> SelectedTemplates;

	FString PluginBaseDirectory;
	FString BackingTemplatePath;

	TSharedPtr<FPluginTemplateDescription> BackingTemplate;
	TSharedPtr<FPluginTemplateDescription> BaseCodeTemplate;

	TSharedPtr<FSlateDynamicImageBrush> IconBrush;
	TSharedPtr<SWidget> CustomHeaderWidget;
};
