// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/SSplineCameraWidget.h"

#include "CameraSpline.h"
#include "CameraSplinePointReference.h"
#include "EditorActorFolders.h"
#include "LevelEditor.h"
#include "SEditorViewport.h"
#include "Selection.h"
#include "WorldPersistentFolders.h"
#include "DebugUtils/DynamicSplineCameraDebugUtils.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Widgets/Input/SSlider.h"

class ACameraSplinePointReference;
class FLevelEditorModule;

void SSplineCameraWidget::Construct(const FArguments& InArgs)
{
	DynamicSplineCameraDebugUtils::Print(TEXT("Creamos nuestro widget"), FColor::Yellow);

	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de EditorActorSubsystem"), FColor::Orange);
	}

	if (!PointReferenceBP)
	{
		PointReferenceBP = Cast<UBlueprint>(
			FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/PointReference")).TryLoad());
		if (PointReferenceBP)
		{
			DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de punto de referencia"), FColor::Magenta);
		}
	}

	if (!CameraSplineBP)
	{
		CameraSplineBP = Cast<UBlueprint>(
			FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/CameraSpline")).TryLoad());
		if (CameraSplineBP)
		{
			DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de Spline de camara"), FColor::Magenta);
		}
	}

	FSlateFontInfo TitleTextFont = GetEmboseedTextFont();
	TitleTextFont.Size = 30;

	FSlateFontInfo BodyTextFont = GetEmboseedTextFont();
	BodyTextFont.Size = 15;

	SelectedSplineCameraText = ConstructTextBlock();
	SelectedSplineCameraText->SetFont(BodyTextFont);

	SelectedReferencePointActorText = ConstructTextBlock();
	SelectedReferencePointActorText->SetFont(BodyTextFont);

	SelectedSplineCameraReferencePointList = ConstructReferencePointsListView();

	CameraPreviewSlider = SNew(SSlider)
		.
		Value(0.0f)
		.
		OnValueChanged(this, &SSplineCameraWidget::OnPreviewSliderValueChanged);

	CreateCameraSplineButton = ConstructButtonBlock(
		TEXT("Spawn Camera Spline"),
		nullptr,
		FSimpleDelegate::CreateRaw(this, &SSplineCameraWidget::OnCreateCameraSplinePressed),
		FSimpleDelegate::CreateRaw(this, &SSplineCameraWidget::OnCreateCameraSplineReleased));

	CreateReferencePointButton = ConstructButtonBlock(
		TEXT("Spawn Point Reference"),
		/*FOnClicked::CreateRaw(this, &SSplineCameraWidget::OnCreateReferencePointClicked)*/ nullptr,
		FSimpleDelegate::CreateRaw(this, &SSplineCameraWidget::OnCreateReferencePointPressed),
		FSimpleDelegate::CreateRaw(this, &SSplineCameraWidget::OnCreateReferencePointReleased));


	ChildSlot
	[
		SNew(SScrollBox)

		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Dynamic spline camera")))
				.Font(TitleTextFont)
				.Justification(ETextJustify::Type::Center)
				.ColorAndOpacity(FColor::White)
				.AutoWrapText(true)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateCameraSplineButton.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SelectedSplineCameraText.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateReferencePointButton.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SelectedReferencePointActorText.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				ConstructButtonBlock(
					TEXT("Override Camera"),
					FOnClicked::CreateRaw(this, &SSplineCameraWidget::OnOverrideSplineReferenceCamera))
				.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.MaxHeight(200.0f)
			.FillHeight(15.0f)
			// .AutoHeight()
			[
				SNew(SScrollBox)

				+ SScrollBox::Slot()
				[
					SelectedSplineCameraReferencePointList.ToSharedRef()
				]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				// .HAlign(HAlign_Center)
				// .VAlign(VAlign_Center)
				.Padding(FMargin(100.0f, 0.0f))
				// .AutoWidth()
				// .FillWidth(0.5f)
				// .MaxWidth(400.0f)
				[
					CameraPreviewSlider.ToSharedRef()
				]

			]

		]
	];
}

void SSplineCameraWidget::OnSpawnTab()
{
	AddDelegates();

	// Una vez creada la interfaz actualizamos detectando si tenemos algo seleccionado
	if (USelection* SelectedActors = GEditor->GetSelectedActors())
	{
		TArray<UObject*> OutSelection;
		SelectedActors->GetSelectedObjects(OutSelection);
		if (OutSelection.Num() > 0)
		{
			OnActorSelected(OutSelection, true);
		}
	}
}

void SSplineCameraWidget::AddDelegates()
{
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	OnActorSelectedDelegateHandle = LevelEditorModule.OnActorSelectionChanged().AddRaw(
		this, &SSplineCameraWidget::OnActorSelected);

	OnActorPropertyChangedDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(
		this, &SSplineCameraWidget::OnActorPropertyChanged);

	if (GEngine)
	{
		OnActorDeletedDelegateHandle = GEngine->OnLevelActorDeleted().
		                                        AddRaw(this, &SSplineCameraWidget::OnActorDeleted);
	}
}

void SSplineCameraWidget::Reset()
{
	SelectedCameraSpline = nullptr;
	SelectedReferencePointActor = nullptr;
	CreatedReferencePointActor = nullptr;
	CreatedCameraSpline = nullptr;

	SelectedSplineCameraReferencePointArray.Empty();

	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

	if (OnActorSelectedDelegateHandle.IsValid())
	{
		LevelEditorModule.OnActorSelectionChanged().Remove(OnActorSelectedDelegateHandle);
	}

	if (OnActorPropertyChangedDelegateHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(OnActorPropertyChangedDelegateHandle);
	}

	if (GEngine && OnActorDeletedDelegateHandle.IsValid())
	{
		GEngine->OnLevelActorDeleted().Remove(OnActorDeletedDelegateHandle);
	}
}

void SSplineCameraWidget::CreateReferencePointArrayFromSplineCamera(ACameraSpline* SplineCamera,
                                                                    TArray<TSharedPtr<FReferencePoint>>& OutArray)
{
	OutArray.Empty();
	for (FReferencePoint ReferencePoint : SplineCamera->ReferencePoints)
	{
		FReferencePoint* NewRef = new FReferencePoint();

		NewRef->CameraPosition = ReferencePoint.CameraPosition;
		NewRef->CameraRotation = ReferencePoint.CameraRotation;
		NewRef->PositionActor = ReferencePoint.PositionActor;

		OutArray.Add(MakeShareable(NewRef));
	}
}

void SSplineCameraWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	switch (CurrentTickState)
	{
	case None:
		break;
	case CreatingSplineCamera:
		TickCreateCameraSpline();
		break;
	case CreatingReferencePoint:
		TickCreateReferencePoint();
		break;
	default: ;
	}
}

void SSplineCameraWidget::TickCreateCameraSpline()
{
	FHitResult Hit;
	if (RaycastViewport(Hit))
	{
		if (CreatedCameraSpline)
		{
			// Lo desplazamos
			CreatedCameraSpline->SetActorLocation(Hit.Location + Hit.Normal * 10.0f);
		}
		else
		{
			// Lo creamos
			CreateCameraSpline();
		}
	}

	GEditor->RedrawAllViewports();
}

void SSplineCameraWidget::TickCreateReferencePoint()
{
	FHitResult Hit;
	if (RaycastViewport(Hit))
	{
		if (CreatedReferencePointActor)
		{
			// Lo desplazamos
			CreatedReferencePointActor->SetActorLocation(Hit.Location + Hit.Normal * 10.0f);
			if (SelectedCameraSpline)
			{
				SelectedCameraSpline->UpdateReferencePointsData();
			}
		}
		else
		{
			// Lo creamos
			CreateReferencePointActor();
		}
	}

	GEditor->RedrawAllViewports();
}

bool SSplineCameraWidget::RaycastViewport(FHitResult& HitResult)
{
	if (FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->
		GetClient()))
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			ViewportClient->Viewport, ViewportClient->GetWorld()->Scene, ViewportClient->EngineShowFlags));

		if (FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily))
		{
			FVector2D MousePosition = FSlateApplication::Get().GetCursorPos();
			TSharedPtr<SEditorViewport> ViewportWidget = ViewportClient->GetEditorViewportWidget();

			FGeometry ViewportGeometry = ViewportWidget->GetTickSpaceGeometry();

			FVector2D LocalMousePosition = ViewportGeometry.AbsoluteToLocal(MousePosition);

			FViewport* Viewport = ViewportClient->Viewport;
			FIntPoint ViewportSize = Viewport->GetSizeXY();

			// Comprobamos que el ratón esté dentro del viewport
			if (LocalMousePosition.X < 0 || LocalMousePosition.Y < 0 || LocalMousePosition.X > ViewportSize.X ||
				LocalMousePosition.Y > ViewportSize.Y)
			{
				return false;
			}

			FVector WorldOrigin;
			FVector WorldDirection;
			SceneView->DeprojectFVector2D(LocalMousePosition, WorldOrigin, WorldDirection);

			FVector End = WorldOrigin + (WorldDirection * 10000.0f);

			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;

			if (ViewportClient->GetWorld()->LineTraceSingleByChannel(
				HitResult, WorldOrigin, End, ECC_Visibility, CollisionParams))
			{
				return HitResult.bBlockingHit;
			}
			return false;
		}
	}
	return false;
}


void SSplineCameraWidget::CreateReferencePointActor()
{
	if (!SelectedCameraSpline) return;

	if (AActor* SpawnedActor = EditorActorSubsystem->SpawnActorFromObject(PointReferenceBP, FVector(0.0f, 0.0f, 0.0f)))
	{
		if (ACameraSplinePointReference* PointReference = Cast<ACameraSplinePointReference>(SpawnedActor))
		{
			CreatedReferencePointActor = PointReference;
			CreatedReferencePointActor->CameraSpline = SelectedCameraSpline;
		}
		else
		{
			DynamicSplineCameraDebugUtils::Print(
				TEXT("Error al intentar hacer casting al punto de referencia"), FColor::Red);
		}
	}
	else
	{
		DynamicSplineCameraDebugUtils::Print(
			TEXT("Error al intentar spawnear el punto de referencia"), FColor::Red);
	}
}

void SSplineCameraWidget::CreateCameraSpline()
{
	if (AActor* SpawnedActor = EditorActorSubsystem->SpawnActorFromObject(CameraSplineBP, FVector(0.0f, 0.0f, 0.0f)))
	{
		if (ACameraSpline* CameraSpline = Cast<ACameraSpline>(SpawnedActor))
		{
			CreatedCameraSpline = CameraSpline;
		}
		else
		{
			DynamicSplineCameraDebugUtils::Print(
				TEXT("Error al intentar hacer casting al CameraSpline"), FColor::Red);
		}
	}
	else
	{
		DynamicSplineCameraDebugUtils::Print(
			TEXT("Error al intentar spawnear el CameraSpline"), FColor::Red);
	}
}

void SSplineCameraWidget::OnActorSelected(const TArray<UObject*>& SelectedActors, bool value)
{
	if (SelectedActors.Num() == 0) return;

	for (UObject* SelectedObject : SelectedActors)
	{
		if (!SelectedObject->IsValidLowLevelFast()) continue;

		if (SelectedObject->IsA(ACameraSpline::StaticClass()))
		{
			DynamicSplineCameraDebugUtils::Print(TEXT("Ponemos nueva seleccion"), FColor::Green);
			SelectedCameraSpline = Cast<ACameraSpline>(SelectedObject);
			CreateReferencePointArrayFromSplineCamera(SelectedCameraSpline, SelectedSplineCameraReferencePointArray);

			RefreshWidgetData();
			return;
		}
		if (SelectedObject->IsA(ACameraSplinePointReference::StaticClass()))
		{
			ACameraSplinePointReference* PointReference = Cast<ACameraSplinePointReference>(SelectedObject);
			if (PointReference->CameraSpline)
			{
				SelectedReferencePointActor = PointReference;
				SelectedCameraSpline = PointReference->CameraSpline;
				CreateReferencePointArrayFromSplineCamera(SelectedCameraSpline,
				                                          SelectedSplineCameraReferencePointArray);

				RefreshWidgetData();
				CameraPreviewSlider->SetValue(0);
				return;
			}
		}
	}

	if (SelectedCameraSpline)
	{
		// SelectedCameraSpline = nullptr;
	}

	RefreshWidgetData();
}

void SSplineCameraWidget::OnActorDeleted(AActor* DeletedActor)
{
	if (DeletedActor == SelectedCameraSpline)
	{
		if (SelectedCameraSpline->ReferencePoints.Num() > 0)
		{
			EAppReturnType::Type confirmedResult = DynamicSplineCameraDebugUtils::ShowMsgDialog(
				EAppMsgType::YesNo, TEXT("Delete also reference points?"));
			if (confirmedResult == EAppReturnType::Yes)
			{
				for (int i = 0; i < SelectedCameraSpline->ReferencePoints.Num(); i++)
				{
					if (SelectedCameraSpline->ReferencePoints[i].PositionActor && !SelectedCameraSpline->ReferencePoints
						[i].PositionActor->IsActorBeingDestroyed())
					{
						SelectedCameraSpline->ReferencePoints[i].PositionActor->Destroy();
					}
				}
			}
		}

		SelectedCameraSpline = nullptr;
		SelectedReferencePointActor = nullptr;
		RefreshWidgetData();
	}
	else if (DeletedActor->IsA(ACameraSplinePointReference::StaticClass()))
	{
		RefreshWidgetData();
	}
}

void SSplineCameraWidget::OnActorPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
	if (Object->IsA(ACameraSpline::StaticClass()))
	{
		if (ACameraSpline* CameraSpline = Cast<ACameraSpline>(Object))
		{
			if (PropertyChangedEvent.Property != nullptr && PropertyChangedEvent.Property->GetName() == "ActorLabel")
			{
				if (CameraSpline->ReferencePoints.Num() == 0) return;

				FName NewPath = FName(TEXT("/CSReferencePoints/") + CameraSpline->GetActorLabel());

				// DynamicSplineCameraDebugUtils::Print(TEXT("OnActorPropertyChanged: ")  + PreviousFolder.GetPath().ToString(), FColor::Black);

				for (int i = 0; i < CameraSpline->ReferencePoints.Num(); i++)
				{
					if (!CameraSpline->ReferencePoints[i].PositionActor) continue;

					FFolder PreviousFolder = CameraSpline->ReferencePoints[i].PositionActor->GetFolder();
					FActorFolders::Get().DeleteFolder(*CameraSpline->GetWorld(), PreviousFolder);

					CameraSpline->ReferencePoints[i].PositionActor->SetFolderPath(
						NewPath);
				}
			}
		}
	}
}

void SSplineCameraWidget::OnCreateReferencePointPressed()
{
	CurrentTickState = ESplneCameraWidgetTickState::CreatingReferencePoint;
}

void SSplineCameraWidget::OnCreateReferencePointReleased()
{
	CurrentTickState = ESplneCameraWidgetTickState::None;

	if (CreatedReferencePointActor)
	{
		FHitResult Hit;
		if (!RaycastViewport(Hit))
		{
			CreatedReferencePointActor->Destroy();
			CreatedReferencePointActor = nullptr;
			return;
		}

		// Lo hemos arrastrado a la escena, le añadimos la información

		FReferencePoint NewReferencePoint = FReferencePoint();

		NewReferencePoint.PositionActor = CreatedReferencePointActor;

		// Obtenemos la posición y rotación de la cámara del editor
		FViewport* ActiveViewport = GEditor->GetActiveViewport();
		if (FEditorViewportClient* EditorViewClient = (ActiveViewport != nullptr)
			                                              ? static_cast<FEditorViewportClient*>(ActiveViewport->
				                                              GetClient())
			                                              : nullptr)
		{
			NewReferencePoint.CameraPosition = EditorViewClient->GetViewLocation();
			NewReferencePoint.CameraRotation = EditorViewClient->GetViewRotation();
		}


		if (SelectedCameraSpline)
		{
			NewReferencePoint.PositionActor->ReferenceIndex = SelectedCameraSpline->ReferencePoints.Num();
			SelectedCameraSpline->ReferencePoints.Add(NewReferencePoint);
			SelectedCameraSpline->UpdateReferencePointsData();
			CreatedReferencePointActor->SetFolderPath(
				FName(TEXT("/CSReferencePoints/") + SelectedCameraSpline->GetActorLabel()));
		}

		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(CreatedReferencePointActor, true);


		CreatedReferencePointActor = nullptr;
	}
}

void SSplineCameraWidget::OnCreateCameraSplinePressed()
{
	CurrentTickState = ESplneCameraWidgetTickState::CreatingSplineCamera;
}

void SSplineCameraWidget::OnCreateCameraSplineReleased()
{
	CurrentTickState = ESplneCameraWidgetTickState::None;

	if (CreatedCameraSpline)
	{
		FHitResult Hit;
		if (!RaycastViewport(Hit))
		{
			CreatedCameraSpline->Destroy();
			CreatedCameraSpline = nullptr;
			return;
		}

		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(CreatedCameraSpline, true);

		CreatedCameraSpline = nullptr;
	}
}

FReply SSplineCameraWidget::OnOverrideSplineReferenceCamera()
{
	if (!SelectedCameraSpline || !SelectedReferencePointActor) return FReply::Handled();;

	//TODO: Pensar algo menos cutre que recorrer todo el array
	// Obtenemos la posición y rotación de la cámara del editor
	FViewport* ActiveViewport = GEditor->GetActiveViewport();
	if (FEditorViewportClient* EditorViewClient = (ActiveViewport != nullptr)
		                                              ? static_cast<FEditorViewportClient*>(ActiveViewport->
			                                              GetClient())
		                                              : nullptr)
	{
		SelectedCameraSpline->ReferencePoints[SelectedReferencePointActor->ReferenceIndex].CameraPosition =
			EditorViewClient->GetViewLocation();
		SelectedCameraSpline->ReferencePoints[SelectedReferencePointActor->ReferenceIndex].CameraRotation =
			EditorViewClient->GetViewRotation();
	}
	else
	{
		return FReply::Handled();;
	}

	SelectedCameraSpline->UpdateReferencePointsData();
	RefreshWidgetData();

	bHasPressedOverrideCamera = true;

	return FReply::Handled();
}

void SSplineCameraWidget::OnReferencePointRowWidgetMouseButtonClicked(TSharedPtr<FReferencePoint> ClickedData)
{
	if (ClickedData.IsValid() && ClickedData.Get()->PositionActor && EditorActorSubsystem && EditorActorSubsystem->
		IsValidLowLevel())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(ClickedData.Get()->PositionActor, true);
	}
}

FReply SSplineCameraWidget::OnPreviewReferencePointButtonClicked(TSharedPtr<FReferencePoint> ClickedData)
{
	if (ClickedData.IsValid() && ClickedData.Get()->PositionActor && EditorActorSubsystem && EditorActorSubsystem->
		IsValidLowLevel())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(ClickedData.Get()->PositionActor, true);

		if (!bHasPressedOverrideCamera)
		{
			SetViewportPosition(ClickedData->CameraPosition);
			SetViewportRotation(ClickedData->CameraRotation);
		}
		else
		{
			bHasPressedOverrideCamera = false;

			if (!SelectedCameraSpline) { return FReply::Handled(); }
			for (int i = 0; i < SelectedCameraSpline->ReferencePoints.Num(); i++)
			{
				if (ClickedData->PositionActor && ClickedData->PositionActor->ReferenceIndex == i)
				{
					SetViewportPosition(SelectedCameraSpline->ReferencePoints[i].CameraPosition);
					SetViewportRotation(SelectedCameraSpline->ReferencePoints[i].CameraRotation);
				}
			}
		}
	}

	return FReply::Handled();
}

void SSplineCameraWidget::OnPreviewSliderValueChanged(float value)
{
	if (!SelectedCameraSpline || !SelectedReferencePointActor || SelectedCameraSpline->ReferencePoints.Num() < 2)
		return;

	int index = SelectedReferencePointActor->ReferenceIndex;
	if (index + 1 > SelectedCameraSpline->ReferencePoints.Num() - 1) return;

	FVector NewPos = FMath::Lerp(SelectedCameraSpline->ReferencePoints[index].CameraPosition,
	                             SelectedCameraSpline->ReferencePoints[index + 1].CameraPosition, value);
	FRotator NewRot = FMath::Lerp(SelectedCameraSpline->ReferencePoints[index].CameraRotation,
	                              SelectedCameraSpline->ReferencePoints[index + 1].CameraRotation, value);

	SetViewportPosition(NewPos);
	SetViewportRotation(NewRot);

	// DynamicSplineCameraDebugUtils::Print(FString::SanitizeFloat(value), FColor::Green);
}

void SSplineCameraWidget::RefreshWidgetData()
{
	if (SelectedCameraSpline && SelectedCameraSpline->IsValidLowLevelFast() && !SelectedCameraSpline->
		IsActorBeingDestroyed())
	{
		if (SelectedSplineCameraText.IsValid())
		{
			SelectedSplineCameraText->SetText(
				FText::FromString(TEXT("Selected Spline Camera: ") + SelectedCameraSpline->GetActorLabel()));
		}
	}
	else
	{
		if (SelectedSplineCameraText.IsValid())
		{
			SelectedSplineCameraText->SetText(FText::FromString(TEXT("No Spline Camera Selected")));
		}
		// SelectedSplineCameraText.Get()->SetText(FText::FromString(TEXT("")));
	}

	if (SelectedReferencePointActor && SelectedReferencePointActor->IsValidLowLevel() &&
		!SelectedReferencePointActor->IsActorBeingDestroyed())
	{
		if (SelectedReferencePointActorText.IsValid())
		{
			SelectedReferencePointActorText->SetText(
				FText::FromString(
					TEXT("Selected Point: ") + FString::FromInt(SelectedReferencePointActor->ReferenceIndex)));
		}
	}
	else
	{
		if (SelectedReferencePointActorText.IsValid())
		{
			SelectedReferencePointActorText->SetText(FText::FromString(TEXT("No Reference Point Selected")));
		}
	}

	SelectedSplineCameraReferencePointList->RebuildList();
}

#pragma region WidgetCreation

TSharedPtr<STextBlock> SSplineCameraWidget::ConstructTextBlock()
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Type::Left)
		.ColorAndOpacity(FColor::White);
}

TSharedPtr<SButton> SSplineCameraWidget::ConstructButtonBlock(FString Text, FOnClicked OnClickedDel,
                                                              FSimpleDelegate OnPressedDel,
                                                              FSimpleDelegate OnReleaseDel)
{
	TSharedPtr<SButton> ConstructedButton = SNew(SButton)
		.ContentPadding(FMargin(5.0f))
		.OnClicked(OnClickedDel)
		.OnPressed(OnPressedDel)
		.OnReleased(OnReleaseDel);

	TSharedPtr<STextBlock> TextBlock = ConstructTextBlock();
	TextBlock->SetText(FText::FromString(Text));
	TextBlock->SetJustification(ETextJustify::Type::Center);

	ConstructedButton->SetContent(TextBlock.ToSharedRef());

	return ConstructedButton;
}

TSharedRef<SListView<TSharedPtr<FReferencePoint>>> SSplineCameraWidget::ConstructReferencePointsListView()
{
	return SNew(SListView<TSharedPtr<FReferencePoint>>)
		.ItemHeight(24.f)
		.ListItemsSource(&SelectedSplineCameraReferencePointArray)
		.OnGenerateRow(this, &SSplineCameraWidget::OnGenerateRowForReferencePointList)
		.OnMouseButtonClick(this, &SSplineCameraWidget::OnReferencePointRowWidgetMouseButtonClicked);
}

TSharedRef<ITableRow> SSplineCameraWidget::OnGenerateRowForReferencePointList(
	TSharedPtr<FReferencePoint> ReferencePoint,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!ReferencePoint.IsValid() || !ReferencePoint->PositionActor || ReferencePoint->PositionActor->
		IsActorBeingDestroyed())
	{
		return SNew(STableRow<TSharedPtr<FReferencePoint>>, OwnerTable);
	}
	FSlateFontInfo AssetNameFont = GetEmboseedTextFont();
	AssetNameFont.Size = 15;

	TSharedPtr<STextBlock> NameTextBlock = ConstructTextBlock();
	NameTextBlock->SetFont(AssetNameFont);
	NameTextBlock->SetText(
		FText::FromString(TEXT("Point" + FString::FromInt(ReferencePoint->PositionActor->ReferenceIndex))));

	TSharedPtr<SButton> PreviewReferencePointButton = ConstructButtonBlock(
		TEXT("View"),
		FOnClicked::CreateRaw(this, &SSplineCameraWidget::OnPreviewReferencePointButtonClicked, ReferencePoint));

	// NameTextBlock->SetText(FText::FromString(TEXT("lkjhbdsafgklhjbsfdga")));

	TSharedRef<ITableRow> ConstructedRow = SNew(STableRow<TSharedPtr<FReferencePoint>>, OwnerTable)
		.Padding(FMargin(5.0f))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.FillWidth(0.05f)
			[
				NameTextBlock.ToSharedRef()
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.FillWidth(0.05f)
			[
				PreviewReferencePointButton.ToSharedRef()
			]
		];

	return ConstructedRow;
}

#pragma endregion

void SSplineCameraWidget::SetViewportPosition(const FVector& Position)
{
	FViewport* ActiveViewport = GEditor->GetActiveViewport();
	if (FEditorViewportClient* EditorViewClient = (ActiveViewport != nullptr)
		                                              ? static_cast<FEditorViewportClient*>(ActiveViewport->
			                                              GetClient())
		                                              : nullptr)
	{
		EditorViewClient->SetViewLocation(Position);
		GEditor->RedrawAllViewports();
	}
}

void SSplineCameraWidget::SetViewportRotation(const FRotator& Rotation)
{
	FViewport* ActiveViewport = GEditor->GetActiveViewport();
	if (FEditorViewportClient* EditorViewClient = (ActiveViewport != nullptr)
		                                              ? static_cast<FEditorViewportClient*>(ActiveViewport->
			                                              GetClient())
		                                              : nullptr)
	{
		EditorViewClient->SetViewRotation(Rotation);
	}
}
