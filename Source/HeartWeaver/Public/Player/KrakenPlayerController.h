// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "KrakenPlayerController.generated.h"


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

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void Move(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	void ToggleCrouch();
	void OnClimbActionStarted(const FInputActionValue & Value);
};
