// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "SplineCameraActor.generated.h"

class ACameraSpline;

/**
 * 
 */
UCLASS()
class DYNAMICSPLINECAMERA_API ASplineCameraActor : public ACameraActor
{
	GENERATED_BODY()

	ASplineCameraActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta = (AllowPrivateAccess))
	AActor* SplineReference;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta = (AllowPrivateAccess))
	ACameraSpline* ActiveCameraSpline;

	int CurrentCameraIndex = 0;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera")
	float LookAtReferenceWeight = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera")
	float CameraSpeed = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera")
	bool bForceFollowOnSpline = false;

private:
	void SetCameraPosition(float DeltaSeconds);

public:
	virtual void Tick(float DeltaSeconds) override;

	void SetActiveSpline(ACameraSpline* Spline);

	void SetCameraIndex(int NewIndex) { CurrentCameraIndex = NewIndex; }
};
