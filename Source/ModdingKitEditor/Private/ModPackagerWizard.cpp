// Fill out your copyright notice in the Description page of Project Settings.


#include "ModPackagerWizard.h"

void SModPackagerWindow::Construct(const FArguments& args)
{
	// Dummy data
	ModList = {
		MakeShared<FString>(TEXT("ModA")),
		MakeShared<FString>(TEXT("ModB")),
		MakeShared<FString>(TEXT("ModC"))
	};

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().FillHeight(1).Padding(4)
		[
			SAssignNew(ListViewWidget, SListView<TSharedPtr<FString>>)
			.ItemHeight(24)
			.ListItemsSource(&ModList)
			.OnGenerateRow(this, &SModPackagerWindow::OnGenerateRow)
		]
	];
}

TSharedRef<ITableRow> SModPackagerWindow::OnGenerateRow(TSharedPtr<FString> Item,
														const TSharedRef<STableViewBase>& Owner)
{
	return SNew(STableRow<TSharedPtr<FString>>, Owner)
	[
		SNew(STextBlock).Text(FText::FromString(*Item))
	];
}
