// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAIPerception.generated.h"

class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorPerceivedDelegate, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorUnperceivedDelegate, AActor*, Actor);

// UPROPERTY(BlueprintAssignable, Category = "Components|Activation")
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSighMeshUpdated, const TArray<FVector2D>&, Points);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEARTWEAVER_API UEnemyAIPerception : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UCapsuleComponent* CapsuleComponent;

	float HalfHeight = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bEyeSighActive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bShowVisionCone = true;

	// Array Cacheado
	TArray<FVector2D> Points;
	TArray<AActor*> ActorsToIgnore = {};

public:
	// Sets default values for this component's properties
	UEnemyAIPerception();

	UFUNCTION(BlueprintCallable)
	void SetEyeSighActive(bool State);

	UPROPERTY(BlueprintAssignable, Category = "Components|SighMesh")
	FSighMeshUpdated OnSighMeshUpdated;

	UPROPERTY(BlueprintAssignable)
	FActorPerceivedDelegate OnActorPerceivedDelegate;

	UPROPERTY(BlueprintAssignable)
	FActorUnperceivedDelegate OnActorUnperceivedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Perception")
	TArray<AActor*> PerceivedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Perception")
	TArray<TSubclassOf<AActor>> ActorFilter;

	// Posición LOCAL de los ojos
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Perception")
	FVector EyesPosition;

	// Offset del ángulo donde empieza la detección
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Perception")
	float EyeSightAngleOffset = 0.0f;

	// Ángulo de visión en GRADOS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Perception")
	float EyeSightAngle = 50.0f;

	// Radio de visión
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Perception")
	float EyeSightRadius = 500.0f;

	// Radio de visión
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Perception")
	float EyeSightPointPrecision = 20.0f;

	void DebugLines(float Duration = 0.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CheckIsInSight(AActor* TargetActor);

	void UpdateConeVisualization();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void CheckPerception(float DeltaTime);

	virtual void OnComponentCreated() override;

	// UFUNCTION(BlueprintImplementableEvent)
	// void UpdateDynamicMesh(const TArray<FVector2D>& Points);
};
