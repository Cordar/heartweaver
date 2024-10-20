// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "SplineCameraTriggerBox.generated.h"

class UBoxComponent;
class ACameraSpline;
class ASplineCameraActor;
/**
 * 
 */
UCLASS()
class DYNAMICSPLINECAMERA_API ASplineCameraTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

	ASplineCameraTriggerBox();

	UPROPERTY()
	UBoxComponent* BoxComponent;

	UPROPERTY()
	ASplineCameraActor* CachedCameraActor;

	bool bCheckForReferencePosition = false;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera Trigger Box")
	ACameraSpline* CameraSpline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera Trigger Box")
	TArray<UClass*> OverlapFilter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera Trigger Box")
	bool bActivateOnlyIfActorCenterIsInsideBox = true;

protected:

	UPROPERTY()
	AActor* ReferenceActor;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);	

	UFUNCTION()
	void OnComponentEndOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	bool IsPointInsideBox(const FVector& Point) const;
};
