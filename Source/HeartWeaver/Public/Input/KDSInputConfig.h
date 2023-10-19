// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KDSInputConfig.generated.h"

class UInputAction;

/**
 * FKDSInputAction
 *
 *	Struct used to map input actions to gameplay input tags.
 */
USTRUCT(BlueprintType)
struct FKDSInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKDSInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UKDSInputConfig();

	UFUNCTION(BlueprintCallable, Category = "KDS|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FKDSInputAction> NativeInputActions;
	
};
