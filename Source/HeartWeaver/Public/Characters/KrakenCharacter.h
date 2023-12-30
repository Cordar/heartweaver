// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KrakenCharacterBase.h"

#include "KrakenCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UKrakenInputConfig;
class USpringArmComponent;
class UCameraComponent;
class UKrakenCharacterMovementComponent;

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

protected:
	virtual void BeginPlay() override;
	virtual bool CanJumpInternal_Implementation() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UKrakenCharacterMovementComponent> KrakenCharacterMovementComponent;

private:
	virtual void InitAbilityActorInfo() override;

	
	void HandleGroundMovementInput(const FInputActionValue& Value);
	void HandleClimbMovementInput(const FInputActionValue& Value);
};
