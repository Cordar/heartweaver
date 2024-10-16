// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "KrakenCharacterBase.h"
#include "KrakenCharacterTypes.h"

#include "KrakenCharacter.generated.h"

class UKrakenPushComponent;
class AKrakenInteractableActor;
struct FInputActionValue;
class UInputMappingContext;
class UKrakenInputConfig;
class USpringArmComponent;
class ACameraActor;
class AKrakenGrabableActor;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class HEARTWEAVER_API AKrakenCharacter : public AKrakenCharacterBase
{
	GENERATED_BODY()

	

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Respawn")
	AActor* LastSavedFloor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Respawn")
	FVector LastSavedRelativeLocation;
	
	AKrakenCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	void ToggleCrouch();
	void Move(const FInputActionValue& Value);
	void MoveStarted();
	void MoveEnded();
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveRandomly();
	
	FORCEINLINE class UKrakenCharacterMovementComponent* GetKrakenCharacterMovementComponent() const { return KrakenCharacterMovementComponent; }

	// Interactable
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	FORCEINLINE AKrakenInteractableActor* GetInteractableActor() const { return KrakenInteractableActor; }
	void SetInteractableActor(AKrakenInteractableActor* InteractableActor);

	// Laying on Floor Intro Scene
	UFUNCTION(BlueprintCallable, Category = "Status")
	bool IsLayingOnFloor() const;
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetLayingOnFloor(bool bNewIsLayingOnFloor);

	FORCEINLINE UKrakenPushComponent* GetPushComponent() const { return KrakenPushComponent; }

	UFUNCTION(BlueprintCallable, Category = "Status")
	EEmotionState GetEmotionState() const { return EmotionState; }
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetEmotionState(const EEmotionState NewEmotionState);

	UFUNCTION(BlueprintCallable)
	void Respawn();


protected:
	virtual void BeginPlay() override;
	virtual bool CanJumpInternal_Implementation() const override;

	UPROPERTY(BlueprintReadWrite, Category = "Camera")
	TObjectPtr<ACameraActor> FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grabable")
	TObjectPtr<AKrakenInteractableActor> KrakenInteractableActor;
	
	//~PlayerLocation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Climbing")
	FVector LastSafeLocation;
	FVector SafeLocation;

	FTimerHandle SaveSafeLocationTimerHandle;
	//~End of PlayerLocation

	EEmotionState EmotionState = EEmotionState::ES_Neutral;

private:
	virtual void InitAbilityActorInfo() override;
	
	void HandleGroundMovementInput(const FInputActionValue& Value);
	void HandlePushMovementInput(const FInputActionValue& Value);
	void HandleClimbMovementInput(const FInputActionValue& Value);

	void SaveLastSafeLocation();

	void ObtainRandomDirection();
	FVector2D RandomDirection;

	bool bIsLayingOnFloor = false;
	bool bIsMoveInputActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category = "Pushable")
	TObjectPtr<UKrakenPushComponent> KrakenPushComponent;
};
