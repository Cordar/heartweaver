// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "KrakenCharacterBase.h"

#include "KrakenCharacter.generated.h"

class AKrakenInteractableActor;
struct FInputActionValue;
class UInputMappingContext;
class UKrakenInputConfig;
class USpringArmComponent;
class ACameraActor;
class UKrakenCharacterMovementComponent;
class AKrakenGrabableActor;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class HEARTWEAVER_API AKrakenCharacter : public AKrakenCharacterBase
{
	GENERATED_BODY()

public:
	AKrakenCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	void ToggleCrouch();
	void Move(const FInputActionValue& Value);
	
	FORCEINLINE class UKrakenCharacterMovementComponent* GetKrakenCharacterMovementComponent() const { return KrakenCharacterMovementComponent; }

	// Interactable
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	FORCEINLINE AKrakenInteractableActor* GetInteractableActor() const { return KrakenInteractableActor; }
	void SetInteractableActor(AKrakenInteractableActor* InteractableActor);

	UFUNCTION(BlueprintCallable, Category = "Status")
	bool IsLayingOnFloor() const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetLayingOnFloor(bool bNewIsLayingOnFloor);


protected:
	virtual void BeginPlay() override;
	virtual bool CanJumpInternal_Implementation() const override;

	UPROPERTY(BlueprintReadWrite, Category = "Camera")
	TObjectPtr<ACameraActor> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UKrakenCharacterMovementComponent> KrakenCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grabable")
	TObjectPtr<AKrakenGrabableActor> KrakenGrabableActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grabable")
	TObjectPtr<AKrakenInteractableActor> KrakenInteractableActor;
	
	//~PlayerLocation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Climbing")
	FVector LastSafeLocation;

	FTimerHandle SaveSafeLocationTimerHandle;
	//~End of PlayerLocation

private:
	virtual void InitAbilityActorInfo() override;
	
	void HandleGroundMovementInput(const FInputActionValue& Value);
	void HandleClimbMovementInput(const FInputActionValue& Value);

	void SaveLastSafeLocation();

	bool bIsLayingOnFloor = true;
};
