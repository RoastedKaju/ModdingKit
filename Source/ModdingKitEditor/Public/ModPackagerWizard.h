// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

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
	TArray<TSharedPtr<FModPluginInfo>> ModList;
	TSharedPtr<SListView<TSharedPtr<FModPluginInfo>>> ListViewWidget;
	TSharedPtr<FModPluginInfo> SelectedModPluginInfo;

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FModPluginInfo> Item, const TSharedRef<STableViewBase>& Owner);
	void OnPluginSelectionChanged(TSharedPtr<FModPluginInfo> SelectedItem, ESelectInfo::Type SelectInfo);
	static TSharedPtr<FSlateBrush> LoadIconBrush(const FString& IconPath);

	FReply OnPackageModButtonClicked();
	bool IsPackageButtonEnabled() const;
};
