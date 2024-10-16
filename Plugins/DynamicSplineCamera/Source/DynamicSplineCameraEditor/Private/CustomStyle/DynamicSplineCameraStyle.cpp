// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/DynamicSplineCameraStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FDynamicSplineCameraStyle::StyleSetName = FName("DynamicSplineCameraEditor");
TSharedPtr<FSlateStyleSet> FDynamicSplineCameraStyle::CreatedSlateStyleSet = nullptr;

void FDynamicSplineCameraStyle::InitializeIcons()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}

TSharedRef<FSlateStyleSet> FDynamicSplineCameraStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("DynamicSplineCameraEditor"))->GetBaseDir() + "/Resources";

	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon16x16(16.0f, 16.0f);
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets",
						new FSlateImageBrush(IconDirectory / "DeleteUnusedAsset.png", Icon16x16));

	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
						new FSlateImageBrush(IconDirectory / "DeleteEmptyFolders.png", Icon16x16));

	CustomStyleSet->Set("ContentBrowser.AdvanceDeletion",
						new FSlateImageBrush(IconDirectory / "AdvanceDeletion.png", Icon16x16));

	return CustomStyleSet;
}

void FDynamicSplineCameraStyle::Shutdown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}
