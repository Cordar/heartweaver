// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "KrakenInteractableActor.h"
#include "GameFramework/Actor.h"
#include "KrakenGrabableActor.generated.h"

class AKrakenCharacter;

UENUM(BlueprintType)
enum class EGrabableType: uint8
{
	Pickup,
	Movable,
	Collectible,
	Interactive,
};


UCLASS()
class HEARTWEAVER_API AKrakenGrabableActor : public AKrakenInteractableActor
{
	GENERATED_BODY()

public:	
	AKrakenGrabableActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabable")
	void OnRelease();
	
	UFUNCTION(BlueprintCallable, Category = "Grabable")
	void Release(AKrakenCharacter* TargetCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabable")
	void OnGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabable")
	FTransform GetTargetCharacterPosition();

	UFUNCTION(BlueprintCallable, Category = "Grabable")
	void Grab(AKrakenCharacter* TargetCharacter);

	UFUNCTION()
	EGrabableType GetGrabableType() const { return GrabableType; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grabable")
	EGrabableType GrabableType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TObjectPtr<UAnimMontage> IdleToGrabMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Climbing")
	TObjectPtr<UAnimMontage> GrabToIdleMontage;
};
