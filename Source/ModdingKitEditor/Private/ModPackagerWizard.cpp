// Fill out your copyright notice in the Description page of Project Settings.


#include "ModPackagerWizard.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializable.h"

void SModPackagerWindow::Construct(const FArguments& args)
{
	// Clear the mod list
	ModList.Empty();

	// Get all enabled plugins
	const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetEnabledPlugins();

	for (const auto& Plugin : Plugins)
	{
		const FString DescriptorFilePath = Plugin->GetDescriptorFileName();
		FString FileContents;
		if (FFileHelper::LoadFileToString(FileContents, *DescriptorFilePath))
		{
			TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(FileContents);
			TSharedPtr<FJsonObject> JsonObject;

			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				FString PluginType;
				if (JsonObject->TryGetStringField(TEXT("Category"), PluginType) && PluginType == TEXT("UGC"))
				{
					UE_LOG(LogTemp, Log, TEXT("Enabled Mod Discovered: %s"), *Plugin->GetName());
					const FString Name = Plugin->GetName();
					const FString Description = JsonObject->GetStringField(TEXT("Description"));
					FString IconPath = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources/Icon128.png"));
					const TSharedPtr<FSlateBrush> Brush = LoadIconBrush(IconPath);

					// Add the mod item to the list
					ModList.Add(MakeShared<FModPluginInfo>(FModPluginInfo{Name, Description, Brush}));
				}
			}
		}
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().FillHeight(1).Padding(4)
		[
			SAssignNew(ListViewWidget, SListView<TSharedPtr<FModPluginInfo>>)
			.ItemHeight(80)
			.ListItemsSource(&ModList)
			.OnGenerateRow(this, &SModPackagerWindow::OnGenerateRow)
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4)
		.HAlign(HAlign_Right)
		[
			SNew(SButton)
			.Text(FText::FromString(TEXT("Package Selected Mod")))
			.OnClicked(this, &SModPackagerWindow::OnPackageModButtonClicked)
		]
	];
}

TSharedRef<ITableRow> SModPackagerWindow::OnGenerateRow(TSharedPtr<FModPluginInfo> Item,
                                                        const TSharedRef<STableViewBase>& Owner)
{
	return SNew(STableRow<TSharedPtr<FModPluginInfo>>, Owner)
		.Padding(FMargin(0, 0, 0, 4))
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.3f))
			.Padding(4)
			[
				SNew(SHorizontalBox)

				// Icon
				+ SHorizontalBox::Slot().AutoWidth().Padding(6)
				[
					SNew(SImage)
					.Image(Item->IconBrush.Get())
				]

				// Name and description
				+ SHorizontalBox::Slot().FillWidth(1).Padding(6)
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 2)
					[
						SNew(STextBlock)
						.Text(FText::FromString(Item->Name))
						.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 16))
					]

					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(Item->Description))
						.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 10))
						.ColorAndOpacity(FSlateColor(FLinearColor::Gray))
					]
				]
			]
		];
}

TSharedPtr<FSlateBrush> SModPackagerWindow::LoadIconBrush(const FString& IconPath)
{
	TSharedPtr<FSlateBrush> Brush;

	if (FPaths::FileExists(IconPath))
	{
		Brush = MakeShared<FSlateDynamicImageBrush>(
			FName(*IconPath), // File path
			FVector2D(64, 64) // Size
		);
	}
	else
	{
		Brush = MakeShared<FSlateDynamicImageBrush>(
			FName(FPaths::EngineContentDir() / TEXT("Editor/Slate/GameProjectDialog/IncludeStarterContent.png")),
			FVector2D(64, 64)
		);
	}

	return Brush;
}

FReply SModPackagerWindow::OnPackageModButtonClicked()
{
	// Your packaging logic here
	return FReply::Handled();
}
