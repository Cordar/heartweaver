// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CameraSplinePointReference.h" // Si es necesario
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/LineBatchComponent.h"
#include "Modules/ModuleInterface.h"


#include "GameFramework/Actor.h"
#include "CameraSpline.generated.h"

class ACameraSplinePointReference;

USTRUCT(BlueprintType)
struct FReferencePoint
{
	GENERATED_USTRUCT_BODY()

	FReferencePoint()
	{
		PositionActor = nullptr;
		CameraPosition = FVector();
		CameraRotation = FRotator();
	}

	FReferencePoint(const FReferencePoint& PointToClone)
	{
		PositionActor = PointToClone.PositionActor;
		CameraPosition = PointToClone.CameraPosition;
		CameraRotation = PointToClone.CameraRotation;
	}

	FReferencePoint(ACameraSplinePointReference* _PositionActor, const FVector& _CameraPosition,
	                const FRotator& _CameraRotation)
	{
		PositionActor = _PositionActor;
		CameraPosition = _CameraPosition;
		CameraRotation = _CameraRotation;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	ACameraSplinePointReference* PositionActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FVector CameraPosition = FVector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FRotator CameraRotation = FRotator();
};

USTRUCT(BlueprintType)
struct FDynamicSplinePoint
{
	GENERATED_USTRUCT_BODY()

	FDynamicSplinePoint()
	{
		Position = FVector();
		CameraPosition = FVector();
		CameraRotation = FRotator();
		DirectionFwrd = FVector();
		DirectionBckrd = FVector();
	}

	FDynamicSplinePoint(const FDynamicSplinePoint& PointToClone)
	{
		Position = PointToClone.Position;
		DirectionFwrd = PointToClone.DirectionFwrd;
		DirectionBckrd = PointToClone.DirectionBckrd;
		CameraPosition = PointToClone.CameraPosition;
		CameraRotation = PointToClone.CameraRotation;
	}

	FDynamicSplinePoint(const FVector& _Position, const FVector& _DirectionFwrd, const FVector& _DirectionBckrd,
	                    const FVector& _CameraPosition, const FRotator& _CameraRotation)
	{
		Position = _Position;
		DirectionFwrd = _DirectionFwrd;
		DirectionBckrd = _DirectionBckrd;
		CameraPosition = _CameraPosition;
		CameraRotation = _CameraRotation;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FVector Position = FVector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FVector DirectionFwrd = FVector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FVector DirectionBckrd = FVector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FVector CameraPosition = FVector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	FRotator CameraRotation = FRotator();
};

UCLASS()
class DYNAMICSPLINECAMERA_API ACameraSpline : public AActor
{
	GENERATED_BODY()

	FTimerHandle Timer;

	int CameraSplineIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category = "Dynamic Spline Camera")
	float CurveAmmount = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category = "Dynamic Spline Camera")
	int CurveSubdivision = 10;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	// float MachullAlpha = 0.5f;

public:
	// Sets default values for this actor's properties
	ACameraSpline();

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	// UPROPERTY()
	// AActor* Reference;

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline")
	// ACameraActor* Camera;

	// UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera Spline")
	UPROPERTY()
	TArray<FReferencePoint> ReferencePoints;

	// UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera Spline")
	UPROPERTY()
	TArray<FDynamicSplinePoint> SplinePoints;

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY()
	ULineBatchComponent* ReferencePointLineBatchComponent;

	UPROPERTY()
	ULineBatchComponent* CameraLineBatchComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void OnConstruction(const FTransform& Transform) override;

	void UpdateReferencePoints();

#pragma region Spline Creation
	void CreateSplinePoints(int Subdivision);
	FVector CalculateQuadraticBezierPoint(float t, const FVector& P0, const FVector& P1, const FVector& P2);
#pragma endregion

	void SetCameraIndexAtPosition(const FVector& Position);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UFUNCTION(BlueprintCallable)
	void GetCameraTransformAtPosition(const FVector& Point, FVector& CameraLocation, FRotator& CameraRotation);

	FVector GetClosestPointBetweenSplinePoints(FVector Position, int PointIndex);
	float GetDistanceFromCurrentSplinePoint(const FVector& Position);
	
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void DrawDebugLines();

	void UpdateReferencePointsData();
	int GetCurrentCameraIndex() const { return CameraSplineIndex; }

	void UpdateIndexFromNewReference(FVector Position);

#if WITH_EDITOR
	void ReestructurateArrayFromDuplicatedReferencePointActor(ACameraSplinePointReference* DuplicatedPoint);
#endif
};
