// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KrakenDynamicObstacle.generated.h"


class UBoxComponent;

UCLASS(Blueprintable, ClassGroup=Camera, meta=(BlueprintSpawnableComponent))
class HEARTWEAVER_API UKrakenDynamicObstacle : public USceneComponent
{
	GENERATED_BODY()

	UFUNCTION()
	void UpdateBoxCollision();

	UFUNCTION()
	bool IsPointInsideBox(const FVector& Point) const;


	// UPROPERTY()
	TMap<class AKrakenNavBox*, TSet<FVector>> PreviousBlockedSetTMap;

public:
	// Sets default values for this component's properties
	UKrakenDynamicObstacle();

	// The camera mesh to show visually where the camera is placed
	UPROPERTY()
	TObjectPtr<class UBoxComponent> BoxCollision;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITORONLY_DATA

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

#endif
};
