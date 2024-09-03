// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UCharacterMovementComponent;
class AKrakenNavBox;
/**
 * 
 */
UCLASS()
class HEARTWEAVER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	ACharacter* ControlledCharacter;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY()
	AKrakenNavBox* NavMeshBox;

	UFUNCTION()
	void ObtainNavMeshBox();

	UPROPERTY()
	TArray<FVector> CurrentPath;


	virtual void BeginPlay() override;

	UFUNCTION()
	void HandlePath(float DeltaSeconds);

	UFUNCTION()
	FVector GetCharacterLocationToNavMeshPoint(FVector NavMeshPoint) const;

	UFUNCTION()
	static int GetClosestPointInArray(FVector Point,TArray<FVector> Array);

private:

	UFUNCTION()
	void Debug_GenerateRandomPath();

	UFUNCTION()
	void Debug_DebugPath();

public:

	virtual void Tick(float DeltaSeconds) override;
};
