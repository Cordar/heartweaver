// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KDSCharacterBase.h"

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
	AKDSCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;
	void ToggleCrouch();

protected:
	virtual void BeginPlay() override;
	virtual bool CanJumpInternal_Implementation() const;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

};