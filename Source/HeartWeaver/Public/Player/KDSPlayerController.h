// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KDSPlayerController.generated.h"


class UInputMappingContext;
class UKDSInputConfig;
class AKDSCharacter;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API AKDSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AKDSPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> HeroContext;

	// Input configuration used by player controller to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UKDSInputConfig> InputConfig;

	// Character being controlled
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AKDSCharacter> ControlledCharacter;

	void Move(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	void ToggleCrouch();
};
