// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * @brief Container to store plugin information
 */
struct FModPluginInfo
{
	FString Name;
	FString Description;
	FString DescriptorFilePath;
	TSharedPtr<FSlateBrush> IconBrush;
};

class SModPackagerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SModPackagerWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& args);

private:
	// All discovered 'UGC' plugins
	TArray<TSharedPtr<FModPluginInfo>> ModList;
	TSharedPtr<SListView<TSharedPtr<FModPluginInfo>>> ListViewWidget;
	// Selected 'UGC' plugin in wizard
	TSharedPtr<FModPluginInfo> SelectedModPluginInfo;
	// Output path for packaged UGC plugin
	TSharedPtr<SEditableTextBox> OutputPathTextBox;
	FString OutputPath;

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FModPluginInfo> Item, const TSharedRef<STableViewBase>& Owner);
	void OnPluginSelectionChanged(TSharedPtr<FModPluginInfo> SelectedItem, ESelectInfo::Type SelectInfo);
	// Load thumbnail from inside the plugin folder
	static TSharedPtr<FSlateBrush> LoadIconBrush(const FString& IconPath);

	FReply OnPackageModButtonClicked();
	bool IsPackageButtonEnabled() const;

	FText GetOutputPathText() const;
	FReply OnBrowseOutputPathClicked();
};
