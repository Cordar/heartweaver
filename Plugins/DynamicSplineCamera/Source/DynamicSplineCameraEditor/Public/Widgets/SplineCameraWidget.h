// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SplineCameraWidget.generated.h"

class SSlider;
struct FReferencePoint;
class ACameraSpline;
class ACameraSplinePointReference;
class ASplineCameraActor;
class ASplineCameraTriggerBox;


UENUM(BlueprintType)
enum ESplineCameraWidgetTickState : uint8
{
	None,
	CreatingReferencePoint,
	CreatingCameraSpline,
	CreatingSplineCameraActor,
	CreatingSplineCameraTriggerBox
};


/**
 * 
 */
UCLASS()
class DYNAMICSPLINECAMERAEDITOR_API USplineCameraWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger Box Default")
	TArray<UClass*> OverlapFilter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger Box Default")
	bool bActivateOnlyIfActorCenterIsInsideBox = true;

	bool bHasPressedOverrideCamera = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Dynamic Spline Camera")
	TEnumAsByte<ESplineCameraWidgetTickState> CurrentTickState = ESplineCameraWidgetTickState::None;
	
#pragma region TabSpawnAndReset
	UPROPERTY()
	UBlueprint* PointReferenceBP;
	UPROPERTY()
	UBlueprint* CameraSplineBP;
	UPROPERTY()
	UBlueprint* SplineCameraActorBP;
	UPROPERTY()
	UBlueprint* SplineCameraTriggerBoxBP;
	
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	void OnSpawnTab();
	void AddDelegates();
	void Reset();
#pragma endregion

#pragma region Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void TickCreateCameraSpline();
	void TickCreateReferencePoint();
	void TickCreateSplineCameraActor();
	void TickCreateSplineCameraTriggerBox();
#pragma endregion

#pragma region Events

	FDelegateHandle OnActorSelectedDelegateHandle;
	FDelegateHandle OnActorDeletedDelegateHandle;
	FDelegateHandle OnActorPropertyChangedDelegateHandle;

	// TArray<TSharedPtr<FReferencePoint>> SelectedSplineCameraReferencePointArray;

	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera")
	ACameraSplinePointReference* SelectedReferencePointActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dynamic Spline Camera")
	ACameraSpline* SelectedCameraSpline;

	UPROPERTY()
	ACameraSplinePointReference* CreatedReferencePointActor;
	void CreateReferencePointActor();

	UPROPERTY()
	ACameraSpline* CreatedCameraSpline;
	void CreateCameraSpline();

	UPROPERTY()
	ASplineCameraActor* CreatedSplineCameraActor;
	void CreateSplineCameraActor();

	UPROPERTY()
	ASplineCameraTriggerBox* CreatedSplineCameraTriggerBox;
	void CreateSplineCameraTriggerBox();

	void OnActorSelected(const TArray<UObject*>& SelectedActors, bool value);
	void OnActorDeleted(AActor* DeletedActor);
	void OnActorPropertyChanged(UObject* Object, struct FPropertyChangedEvent& PropertyChangedEvent);
	
#pragma endregion
	
#pragma region SpawnActorButtons

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnCameraActorPressed();
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnCameraActorReleased();

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnCameraSplinePressed();
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnCameraSplineReleased();

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnSplineCameraTriggerBoxPressed();
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnSplineCameraTriggerBoxReleased();

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnReferencePointPressed();
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSpawnReferencePointReleased();

#pragma  endregion

#pragma region ReferencePointList

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnSelectReferencePointClicked(FReferencePoint Point);
	
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnViewReferencePointClicked(FReferencePoint Point);

#pragma endregion

#pragma  region PreviewSlider
	
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OnPreviewSliderValueChanged(float Value);

#pragma  endregion

#pragma region Config Data
	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void SaveConfigData();

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void LoadConfigData();
#pragma  endregion

#pragma region Viewport
	static void SetViewportPosition(const FVector& Position);
	static void SetViewportRotation(const FRotator& Rotation);
	static bool RaycastViewport(FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "Dynamic Spline Camera")
	void OverrideCamera();
#pragma endregion

	UFUNCTION(BlueprintImplementableEvent, Category = "Dynamic Spline Camera")
	void RefreshWidgetData();

	// void CreateReferencePointArrayFromSplineCamera(ACameraSpline* SplineCamera,
	// 										   TArray<TSharedPtr<FReferencePoint>>& OutArray);
	
	// FSlateFontInfo GetEmboseedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }

};
