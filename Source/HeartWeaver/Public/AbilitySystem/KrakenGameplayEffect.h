// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "KrakenGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Utils")
	void SetAssetTagInAssetTagsComponent(FGameplayTag InTag);
};
