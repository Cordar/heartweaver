// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "SplineCameraTriggerBox.generated.h"

class ACameraSpline;
/**
 * 
 */
UCLASS()
class DYNAMICSPLINECAMERA_API ASplineCameraTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

	ASplineCameraTriggerBox();

	bool bCheckForReferencePosition = false;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline Camera")
	ACameraSpline* CameraSpline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline Camera")
	TArray<UClass*> OverlapFilter; 

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
