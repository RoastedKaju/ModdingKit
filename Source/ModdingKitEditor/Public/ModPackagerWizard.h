// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


class SModPackagerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SModPackagerWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& args);

private:
	TArray<TSharedPtr<FString>> ModList;
	TSharedPtr<SListView<TSharedPtr<FString>>> ListViewWidget;

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& Owner);
};


