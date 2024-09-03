// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAIPerception.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorPerceivedDelegate, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorUnperceivedDelegate, AActor*, Actor);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HEARTWEAVER_API UEnemyAIPerception : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyAIPerception();

	UPROPERTY(BlueprintAssignable)
	FActorPerceivedDelegate OnActorPerceivedDelegate;

	UPROPERTY(BlueprintAssignable)
	FActorUnperceivedDelegate OnActorUnperceivedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Perception")
	TArray<AActor*> PerceivedActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Perception")
	TArray<TSubclassOf<AActor>> ActorFilter;

	// Posición LOCAL de los ojos
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Perception")
	FVector EyesPosition;

	// Ángulo de visión en GRADOS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Perception")
	float EyeSightAngle = 50.0f;

	// Radio de visión
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI Perception")
	float EyeSightRadius = 500.0f;

	void DebugLines(float Duration = 0.0f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool CheckIsInSight(AActor* TargetActor);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void CheckPerception(float DeltaTime);

	virtual void OnComponentCreated() override;


};
