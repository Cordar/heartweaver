// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/DynamicSplineCameraStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FDynamicSplineCameraStyle::StyleSetName = FName("DynamicSplineCamera");
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

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("DynamicSplineCamera"))->GetBaseDir() + "/Resources";

	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon16x16(16.0f, 16.0f);
	CustomStyleSet->Set("DynamicSplineCamera.CategoryIcon",
						new FSlateImageBrush(IconDirectory / "DynamicSplineCameraIcon16.png", Icon16x16));


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
