// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraSplinePointReference.generated.h"

class ACameraSpline;

UCLASS()
class DYNAMICSPLINECAMERA_API ACameraSplinePointReference : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess), Category = "Dynamic Spline Camera")
	USceneComponent* Root;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess), Category = "Dynamic Spline Camera")
	UBillboardComponent* Billboard;
#endif
public:	
	// Sets default values for this actor's properties
	ACameraSplinePointReference();

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Spline Point Reference")
	UPROPERTY()
	ACameraSpline* CameraSpline;

	UPROPERTY(BlueprintReadOnly, Category = "Dynamic Spline Camera")
	int ReferenceIndex = -1;
protected:

#if WITH_EDITOR
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

	virtual void PostEditMove(bool bFinished) override;

	virtual void Destroyed() override;
	// virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// virtual void OnConstruction(const FTransform& Transform) override;
public:
};
