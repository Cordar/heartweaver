// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SSlider;
struct FReferencePoint;
class ACameraSpline;
class ACameraSplinePointReference;

UENUM(BlueprintType)
enum ESplneCameraWidgetTickState : uint8
{
	None,
	CreatingReferencePoint,
	CreatingSplineCamera
};

/**
 * 
 */
class DYNAMICSPLINECAMERAEDITOR_API SSplineCameraWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SSplineCameraWidget)
		{
		}

		// SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore)
		// SLATE_ARGUMENT(FString, CurrentSelectedFolder)

	SLATE_END_ARGS()

	bool bHasPressedOverrideCamera = false;

	ESplneCameraWidgetTickState CurrentTickState = ESplneCameraWidgetTickState::None;

	void AddDelegates();
	void CreateReferencePointArrayFromSplineCamera(ACameraSpline* SplineCamera,
	                                               TArray<TSharedPtr<FReferencePoint>>& OutArray);
	UBlueprint* PointReferenceBP;
	UBlueprint* CameraSplineBP;

#pragma region Widgets

	TSharedPtr<SButton> CreateCameraSplineButton;
	TSharedPtr<SButton> CreateReferencePointButton;

	TSharedPtr<STextBlock> SelectedSplineCameraText;
	TSharedPtr<STextBlock> SelectedReferencePointActorText;
	TSharedPtr<SSlider> CameraPreviewSlider;

	TSharedPtr<SListView<TSharedPtr<FReferencePoint>>> SelectedSplineCameraReferencePointList;

#pragma endregion
#pragma region ActorSelection
	
	FDelegateHandle OnActorSelectedDelegateHandle;
	FDelegateHandle OnActorDeletedDelegateHandle;
	FDelegateHandle OnActorPropertyChangedDelegateHandle;

	class UEditorActorSubsystem* EditorActorSubsystem;

	ACameraSplinePointReference* SelectedReferencePointActor;
	ACameraSpline* SelectedCameraSpline;

	ACameraSplinePointReference* CreatedReferencePointActor;
	void CreateReferencePointActor();
	
	ACameraSpline* CreatedCameraSpline;
	void CreateCameraSpline();

	void OnActorSelected(const TArray<UObject*>& SelectedActors, bool value);
	void OnActorDeleted(AActor* DeletedActor);
	void OnActorPropertyChanged(UObject* Object, struct FPropertyChangedEvent& PropertyChangedEvent);

	TArray<TSharedPtr<FReferencePoint>> SelectedSplineCameraReferencePointArray;
#pragma  endregion
	void OnCreateReferencePointPressed();
	void OnCreateReferencePointReleased();

	void OnCreateCameraSplinePressed();
	void OnCreateCameraSplineReleased();
	
	FReply OnOverrideSplineReferenceCamera();

	void OnReferencePointRowWidgetMouseButtonClicked(TSharedPtr<FReferencePoint> ClickedData);
	FReply OnPreviewReferencePointButtonClicked(TSharedPtr<FReferencePoint> ClickedData);
	void OnPreviewSliderValueChanged(float value);
	
	void RefreshWidgetData();


#pragma region WidgetCreation
	// TSharedRef<SButton> ConstrucButton(const FArguments& InArgs);
	// TSharedRef<STextBlock> ConstructText(const FString& TextContent);
	
	TSharedPtr<STextBlock> ConstructTextBlock();
	TSharedPtr<SButton> ConstructButtonBlock(FString Text, FOnClicked OnClickedDel, FSimpleDelegate OnPressedDel = nullptr, FSimpleDelegate OnReleaseDel = nullptr);
	TSharedRef<SListView<TSharedPtr<FReferencePoint>>> ConstructReferencePointsListView();
	TSharedRef<ITableRow> OnGenerateRowForReferencePointList(TSharedPtr<FReferencePoint> ReferencePoint,
																	const TSharedRef<STableViewBase>& OwnerTable);
#pragma endregion

	static void SetViewportPosition(const FVector& Position);
	static void SetViewportRotation(const FRotator& Rotation);

public:
	void Construct(const FArguments& InArgs);

	void OnSpawnTab();
	void Reset();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void TickCreateCameraSpline();
	void TickCreateReferencePoint();

	static bool RaycastViewport(FHitResult& Hit);

	FSlateFontInfo GetEmboseedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }
};
