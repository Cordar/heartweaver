// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "KrakenLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	UKrakenLocalPlayer();

public:
	UFUNCTION()
	UKrakenSettingsLocal* GetLocalSettings() const;	
};
