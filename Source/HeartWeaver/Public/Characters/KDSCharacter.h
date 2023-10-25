// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KDSCharacterBase.h"
#include "InputActionValue.h"

#include "KDSCharacter.generated.h"

class UInputMappingContext;
class UKDSInputConfig;
class USpringArmComponent;
class UCameraComponent;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class HEARTWEAVER_API AKDSCharacter : public AKDSCharacterBase
{
	GENERATED_BODY()

public:
	AKDSCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input configuration used by character to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KDS|Input")
	TObjectPtr<UKDSInputConfig> InputConfig;

	void Move(const FInputActionValue& Value);
	void ToggleCrouch(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

};
