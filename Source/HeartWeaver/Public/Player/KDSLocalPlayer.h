// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "KDSLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKDSLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	UKDSLocalPlayer();

public:
	UFUNCTION()
	UKDSSettingsLocal* GetLocalSettings() const;	
};
