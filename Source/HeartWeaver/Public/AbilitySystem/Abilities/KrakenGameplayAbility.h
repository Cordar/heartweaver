// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "KrakenGameplayAbility.generated.h"

class AKrakenCharacter;
/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;

protected:
	UFUNCTION(BlueprintCallable, Category = Ability)
	AKrakenCharacter* GetKrakenCharacterFromActorInfo() const;
	
};
