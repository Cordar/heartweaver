// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "Interfaces/KrakenActivableObject.h"
#include "KrakenInteractableActor.generated.h"

class AKrakenCharacter;

UCLASS()
class HEARTWEAVER_API AKrakenInteractableActor : public AActor, public IKrakenActivableObject, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	AKrakenInteractableActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void WasSetAsTarget();
	
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void SetAsTarget(AKrakenCharacter* TargetCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void WasUnsetAsTarget();
	
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void UnsetAsTarget(AKrakenCharacter* TargetCharacter);

protected:
	virtual void BeginPlay() override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; return; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	FGameplayTagContainer GameplayTags;
};
