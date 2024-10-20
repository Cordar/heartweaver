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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera Trigger Box", meta = (ToolTip = "Spline that will get activated when an actor contained in OverlapFilter enter the box"))
	ACameraSpline* CameraSpline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera Trigger Box", meta = (ToolTip = "Whenever an actor begins overlap with the box, it will check if it's contained within this filter"))
	TArray<UClass*> OverlapFilter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera Trigger Box", meta = (ToolTip = "If true, the spline will only activate when the actor center is inside the box; On begin overlap otherwise"))
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
