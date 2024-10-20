// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DYNAMICSPLINECAMERAEDITOR_API FDynamicSplineCameraStyle
{
public:
	static void InitializeIcons();
	static void Shutdown();

private:
	static FName StyleSetName;

	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;

public:
	static FName GetStyleSetName() { return StyleSetName; }
};
