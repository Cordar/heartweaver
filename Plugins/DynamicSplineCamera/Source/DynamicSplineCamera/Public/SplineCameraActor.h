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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta = (AllowPrivateAccess), meta = (ToolTip = "Actor reference to check our desired camera position and rotation, usually the player character"))
	AActor* SplineReference;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta = (AllowPrivateAccess), meta = (ToolTip = "Active Camera Spline"))
	ACameraSpline* ActiveCameraSpline;

	int CurrentCameraIndex = 0;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta=(ClampMax = 1.0f, ClampMin = 0.0f), meta = (ToolTip = "How much will the camera look at the reference at runtime"))
	float LookAtReferenceWeight = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta = (ToolTip = "Camera Speed"), meta=(ClampMin = 0.0f))
	float CameraSpeed = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera", meta = (ToolTip = "If true, the camera will be force to follow the spline and won't be able to interpolate position out of the spline"))
	bool bForceFollowOnSpline = false;

private:
	void SetCameraPosition(float DeltaSeconds);

public:
	virtual void Tick(float DeltaSeconds) override;

	void SetActiveSpline(ACameraSpline* Spline);

	void SetCameraIndex(int NewIndex) { CurrentCameraIndex = NewIndex; }
};
