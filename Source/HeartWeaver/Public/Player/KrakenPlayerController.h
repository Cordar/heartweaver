// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KrakenPlayerController.generated.h"


class UKrakenAbilitySystemComponent;
struct FGameplayTag;
class UInputMappingContext;
class UKrakenInputConfig;
class AKrakenCharacter;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API AKrakenPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AKrakenPlayerController();
	
	virtual void UpdateRotation(const float DeltaTime) override;
	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityRelativeRotation(const FRotator& Rotation, const FVector& GravityDirection);
 
	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityWorldRotation(const FRotator& Rotation, const FVector& GravityDirection);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> HeroContext;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AKrakenCharacter> ControlledCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UKrakenInputConfig> InputConfig;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UKrakenAbilitySystemComponent> KrakenAbilitySystemComponent;

	FVector LastFrameGravity = FVector::ZeroVector;

	UKrakenAbilitySystemComponent* GetKrakenAbilitySystemComponent();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void Move(const FInputActionValue& Value);
	void MoveStarted();
	void MoveEnded();
	void Jump();
	void StopJumping();
	void ToggleCrouch();
	void OnClimbActionStarted(const FInputActionValue & Value);
};
