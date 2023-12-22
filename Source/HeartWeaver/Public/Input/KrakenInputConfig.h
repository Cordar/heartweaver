// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KrakenInputConfig.generated.h"

class UInputAction;

/**
 * FKrakenInputAction
 *
 *	Struct used to map input actions to gameplay input tags.
 */
USTRUCT(BlueprintType)
struct FKrakenInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UKrakenInputConfig();

	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	
	UFUNCTION(BlueprintCallable)
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	const UInputAction* FindInputActionForTag(const TArray<FKrakenInputAction>& InputActions, const FGameplayTag& InputTag, bool bLogNotFound) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FKrakenInputAction> NativeInputActions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FKrakenInputAction> AbilityInputActions;
	
};
