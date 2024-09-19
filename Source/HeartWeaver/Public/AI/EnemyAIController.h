// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyState.h"
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

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category = "AI")
	// UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = "AI")
	bool bEvadeObstacles = true;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY()
	ACharacter* ControlledCharacter;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess), Category = "AI")
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY()
	AKrakenNavBox* NavMeshBox;

	UFUNCTION()
	void ObtainNavMeshBox();	

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void HandlePathMovement(float DeltaSeconds);

	UFUNCTION()
	void OnPerceptActor(AActor* Actor);

	UFUNCTION()
	void OnUnperceptActor(AActor* Actor);

private:
	UFUNCTION()
	void Debug_GenerateRandomPath();

	UFUNCTION()
	void Debug_DebugPath();

public:

	AEnemyAIController();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "AI")
	TArray<FVector> CurrentPath;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStateChange(EEnemyState PreviousState, EEnemyState NewState);

	UFUNCTION(BlueprintCallable)
	void SetState(EEnemyState NewState);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEnemyState GetState();

	UFUNCTION(BlueprintCallable)
	void CreatePathToTarget();

	UFUNCTION(BlueprintCallable)
	void CreatePathToLocationTarget();

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetCharacterLocationToNavMeshPoint(const FVector& NavMeshPoint) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int GetClosestPointInArray(const FVector& Point, TArray<FVector> Array);
};
