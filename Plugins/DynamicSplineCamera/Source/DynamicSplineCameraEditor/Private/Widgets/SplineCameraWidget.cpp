// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SplineCameraWidget.h"

#include "CameraSpline.h"
#include "CameraSplinePointReference.h"
#include "EditorActorFolders.h"
#include "LevelEditor.h"
#include "SEditorViewport.h"
#include "Selection.h"
#include "SplineCameraActor.h"
#include "SplineCameraTriggerBox.h"
#include "DebugUtils/DynamicSplineCameraDebugUtils.h"
#include "Subsystems/EditorActorSubsystem.h"

void USplineCameraWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnSpawnTab();	
}

void USplineCameraWidget::BeginDestroy()
{
	Super::BeginDestroy();

	Reset();
}

#pragma region Tick

void USplineCameraWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
	switch (CurrentTickState)
	{
	case None:
		break;
	case CreatingCameraSpline:
		TickCreateCameraSpline();
		break;
	case CreatingReferencePoint:
		TickCreateReferencePoint();
		break;
	case CreatingSplineCameraActor:
		TickCreateSplineCameraActor();
		break;
	case CreatingSplineCameraTriggerBox:
		TickCreateSplineCameraTriggerBox();
		break;
	default: ;
	}
}

void USplineCameraWidget::TickCreateCameraSpline()
{
	FHitResult Hit;
	if (RaycastViewport(Hit))
	{
		if (CreatedCameraSpline)
		{
			// Lo desplazamos
			CreatedCameraSpline->SetActorLocation(Hit.Location + Hit.Normal * 50.0f);
		}
		else
		{
			// Lo creamos
			CreateCameraSpline();
		}
	}

	GEditor->RedrawAllViewports();
}

void USplineCameraWidget::TickCreateReferencePoint()
{
	FHitResult Hit;
	if (RaycastViewport(Hit))
	{
		if (CreatedReferencePointActor)
		{
			// Lo desplazamos
			CreatedReferencePointActor->SetActorLocation(Hit.Location + Hit.Normal * 20.0f);
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

void USplineCameraWidget::TickCreateSplineCameraActor()
{
	FHitResult Hit;
	if (RaycastViewport(Hit))
	{
		if (CreatedSplineCameraActor)
		{
			// Lo desplazamos
			CreatedSplineCameraActor->SetActorLocation(Hit.Location + Hit.Normal * 100.0f);
		}
		else
		{
			// Lo creamos
			CreateSplineCameraActor();
		}
	}

	GEditor->RedrawAllViewports();
}

void USplineCameraWidget::TickCreateSplineCameraTriggerBox()
{
	FHitResult Hit;
	if (RaycastViewport(Hit))
	{
		if (CreatedSplineCameraTriggerBox)
		{
			// Lo desplazamos
			CreatedSplineCameraTriggerBox->SetActorLocation(Hit.Location + Hit.Normal * 100.0f);
		}
		else
		{
			// Lo creamos
			CreateSplineCameraTriggerBox();
		}
	}

	GEditor->RedrawAllViewports();
}

#pragma endregion

#pragma region TabSpawnAndReset

void USplineCameraWidget::OnSpawnTab()
{
	LoadConfigData();
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

	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		// DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de EditorActorSubsystem"), FColor::Orange);
	}

	if (!PointReferenceBP)
	{
		PointReferenceBP = Cast<UBlueprint>(
			FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/PointReference")).TryLoad());
		// if (PointReferenceBP)
		// {
		// 	DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de punto de referencia"), FColor::Magenta);
		// }
	}

	if (!CameraSplineBP)
	{
		CameraSplineBP = Cast<UBlueprint>(
			FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/CameraSpline")).TryLoad());
		// if (CameraSplineBP)
		// {
		// 	DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de Spline de camara"), FColor::Magenta);
		// }
	}

	if (!SplineCameraActorBP)
	{
		SplineCameraActorBP = Cast<UBlueprint>(
			FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/SplineCameraActor")).TryLoad());
		// if (SplineCameraActorBP)
		// {
		// 	DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de Spline camera actor"), FColor::Cyan);
		// }
	}

	if (!SplineCameraTriggerBoxBP)
	{
		SplineCameraTriggerBoxBP = Cast<UBlueprint>(
			FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/SplineCameraTriggerBox")).TryLoad());
		// if (SplineCameraTriggerBoxBP)
		// {
		// 	DynamicSplineCameraDebugUtils::Print(TEXT("Carga completa de Spline Camera Trigger Box"), FColor::Yellow);
		// }
	}

	RefreshWidgetData();
}

void USplineCameraWidget::AddDelegates()
{
	FLevelEditorModule& LevelEditorModule =
	FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	
	OnActorSelectedDelegateHandle = LevelEditorModule.OnActorSelectionChanged().AddUObject(
		this, &USplineCameraWidget::OnActorSelected);
	
	OnActorPropertyChangedDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(
		this, &USplineCameraWidget::OnActorPropertyChanged);
	
	if (GEngine)
	{
		OnActorDeletedDelegateHandle = GEngine->OnLevelActorDeleted().
												AddUObject(this, &USplineCameraWidget::OnActorDeleted);
	}
}

void USplineCameraWidget::Reset()
{
	SelectedCameraSpline = nullptr;
	SelectedReferencePointActor = nullptr;
	CreatedReferencePointActor = nullptr;
	CreatedCameraSpline = nullptr;

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

#pragma endregion

#pragma region Events

void USplineCameraWidget::CreateReferencePointActor()
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

void USplineCameraWidget::CreateCameraSpline()
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

void USplineCameraWidget::CreateSplineCameraActor()
{
	if (AActor* SpawnedActor = EditorActorSubsystem->SpawnActorFromObject(
	SplineCameraActorBP, FVector(0.0f, 0.0f, 0.0f)))
	{
		if (ASplineCameraActor* SplineCameraActor = Cast<ASplineCameraActor>(SpawnedActor))
		{
			CreatedSplineCameraActor = SplineCameraActor;
		}
		else
		{
			DynamicSplineCameraDebugUtils::Print(
				TEXT("Error al intentar hacer casting al SplineCameraActor"), FColor::Red);
		}
	}
	else
	{
		DynamicSplineCameraDebugUtils::Print(
			TEXT("Error al intentar spawnear el SplineCameraActor"), FColor::Red);
	}
}

void USplineCameraWidget::CreateSplineCameraTriggerBox()
{
	if (AActor* SpawnedActor = EditorActorSubsystem->SpawnActorFromObject(
	SplineCameraTriggerBoxBP, FVector(0.0f, 0.0f, 0.0f)))
	{
		if (ASplineCameraTriggerBox* SplineCameraTriggerBox = Cast<ASplineCameraTriggerBox>(SpawnedActor))
		{
			CreatedSplineCameraTriggerBox = SplineCameraTriggerBox;
		}
		else
		{
			DynamicSplineCameraDebugUtils::Print(
				TEXT("Error al intentar hacer casting al SplineCameraTriggerBox"), FColor::Red);
		}
	}
	else
	{
		DynamicSplineCameraDebugUtils::Print(
			TEXT("Error al intentar spawnear el SplineCameraTriggerBox"), FColor::Red);
	}
}

void USplineCameraWidget::OnActorSelected(const TArray<UObject*>& SelectedActors, bool value)
{
	if (SelectedActors.Num() == 0) return;

	for (UObject* SelectedObject : SelectedActors)
	{
		if (!SelectedObject->IsValidLowLevelFast()) continue;

		if (SelectedObject->IsA(ACameraSpline::StaticClass()))
		{
			if (SelectedObject != SelectedCameraSpline)
			{
				SelectedReferencePointActor = nullptr;
			}
			// DynamicSplineCameraDebugUtils::Print(TEXT("Ponemos nueva seleccion"), FColor::Green);
			SelectedCameraSpline = Cast<ACameraSpline>(SelectedObject);

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

				RefreshWidgetData();

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

void USplineCameraWidget::OnActorDeleted(AActor* DeletedActor)
{
		if (DeletedActor->IsA(ACameraSpline::StaticClass()))
	{
		if (ACameraSpline* DeletedCameraSpline = Cast<ACameraSpline>(DeletedActor))
		{
			if (DeletedCameraSpline->ReferencePoints.Num() > 0)
			{
				EAppReturnType::Type confirmedResult = DynamicSplineCameraDebugUtils::ShowMsgDialog(
					EAppMsgType::YesNo, TEXT("Delete also reference points?"));
				if (confirmedResult == EAppReturnType::Yes)
				{
					// Maybe change this back to for loop so we don't rearrange array after deletion
					for (int i = 0; i < DeletedCameraSpline->ReferencePoints.Num(); i++)
					{
						if (DeletedCameraSpline->ReferencePoints[i].PositionActor && !DeletedCameraSpline->
							ReferencePoints[i].PositionActor->IsActorBeingDestroyed())
						{
							DeletedCameraSpline->ReferencePoints[i].PositionActor->Destroy();

							// When we delete a Reference point, the reference point autoremoves itself from the array
							i--;
						}
					}
				}
			}

			if (DeletedCameraSpline == SelectedCameraSpline)
			{
				SelectedCameraSpline = nullptr;
				SelectedReferencePointActor = nullptr;
			}
			RefreshWidgetData();
		}
	}
	else if (DeletedActor->IsA(ACameraSplinePointReference::StaticClass()))
	{
		if (ACameraSplinePointReference* PointReference = Cast<ACameraSplinePointReference>(DeletedActor))
		{
			if (SelectedCameraSpline && PointReference->CameraSpline == SelectedCameraSpline && SelectedCameraSpline->ReferencePoints.Num() > 0)
			{
				int SelectedIndex = -1;
				for (int i = PointReference->ReferenceIndex - 1; i >= 0; i--)
				{
					if (i < SelectedCameraSpline->ReferencePoints.Num() && SelectedCameraSpline->ReferencePoints[i].
						PositionActor
						&& !SelectedCameraSpline->ReferencePoints[i].PositionActor->IsActorBeingDestroyed())
					{
						SelectedIndex = i;
						break;
					}
				}
				if (SelectedIndex >= 0)
				{
					EditorActorSubsystem->SelectNothing();
					EditorActorSubsystem->SetActorSelectionState(
						SelectedCameraSpline->ReferencePoints[SelectedIndex].PositionActor, true);
				}
			}
		}
		RefreshWidgetData();
	}
}

void USplineCameraWidget::OnActorPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
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

#pragma endregion

#pragma region SpawnActorButtons

void USplineCameraWidget::OnSpawnCameraActorPressed()
{
	CurrentTickState = CreatingSplineCameraActor;
}

void USplineCameraWidget::OnSpawnCameraActorReleased()
{
	CurrentTickState = ESplineCameraWidgetTickState::None;

	if (CreatedSplineCameraActor)
	{
		FHitResult Hit;
		if (!RaycastViewport(Hit))
		{
			CreatedSplineCameraActor->Destroy();
			CreatedSplineCameraActor = nullptr;
			return;
		}

		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(CreatedSplineCameraActor, true);

		CreatedSplineCameraActor = nullptr;
	}
}

void USplineCameraWidget::OnSpawnCameraSplinePressed()
{
	CurrentTickState = ESplineCameraWidgetTickState::CreatingCameraSpline;
}

void USplineCameraWidget::OnSpawnCameraSplineReleased()
{
	CurrentTickState = ESplineCameraWidgetTickState::None;

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

void USplineCameraWidget::OnSpawnSplineCameraTriggerBoxPressed()
{
	CurrentTickState = CreatingSplineCameraTriggerBox;
}

void USplineCameraWidget::OnSpawnSplineCameraTriggerBoxReleased()
{
	CurrentTickState = None;

	if (CreatedSplineCameraTriggerBox)
	{
		FHitResult Hit;
		if (!RaycastViewport(Hit))
		{
			CreatedSplineCameraTriggerBox->Destroy();
			CreatedSplineCameraTriggerBox = nullptr;
			return;
		}

		CreatedSplineCameraTriggerBox->OverlapFilter = OverlapFilter;
		CreatedSplineCameraTriggerBox->bActivateOnlyIfActorCenterIsInsideBox = bActivateOnlyIfActorCenterIsInsideBox;
		
		if (SelectedCameraSpline)
		{
			CreatedSplineCameraTriggerBox->CameraSpline = SelectedCameraSpline;
		}

		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(CreatedSplineCameraTriggerBox, true);

		CreatedSplineCameraTriggerBox = nullptr;
	}
}

void USplineCameraWidget::OnSpawnReferencePointPressed()
{
	CurrentTickState = ESplineCameraWidgetTickState::CreatingReferencePoint;
}

void USplineCameraWidget::OnSpawnReferencePointReleased()
{
	CurrentTickState = ESplineCameraWidgetTickState::None;

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

#pragma endregion

#pragma region ReferencePointList

void USplineCameraWidget::OnSelectReferencePointClicked(FReferencePoint Point)
{
	if (Point.PositionActor && EditorActorSubsystem && EditorActorSubsystem->
	IsValidLowLevel())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(Point.PositionActor, true);
	}
}

void USplineCameraWidget::OnViewReferencePointClicked(FReferencePoint Point)
{
	if (Point.PositionActor && EditorActorSubsystem && EditorActorSubsystem->
	IsValidLowLevel())
	{
		EditorActorSubsystem->SelectNothing();
		EditorActorSubsystem->SetActorSelectionState(Point.PositionActor, true);

		if (!bHasPressedOverrideCamera)
		{
			SetViewportPosition(Point.CameraPosition);
			SetViewportRotation(Point.CameraRotation);
		}
		else
		{
			bHasPressedOverrideCamera = false;

			if (!SelectedCameraSpline) { return; }
			for (int i = 0; i < SelectedCameraSpline->ReferencePoints.Num(); i++)
			{
				if (Point.PositionActor && Point.PositionActor->ReferenceIndex == i)
				{
					SetViewportPosition(SelectedCameraSpline->ReferencePoints[i].CameraPosition);
					SetViewportRotation(SelectedCameraSpline->ReferencePoints[i].CameraRotation);
				}
			}
		}
	}
}

#pragma endregion

#pragma  region PreviewSlider

void USplineCameraWidget::OnPreviewSliderValueChanged(float Value)
{
	if (!SelectedCameraSpline || !SelectedReferencePointActor || SelectedCameraSpline->ReferencePoints.Num() < 2)
		return;

	int index = SelectedReferencePointActor->ReferenceIndex;
	if (index + 1 > SelectedCameraSpline->ReferencePoints.Num() - 1) return;

	FVector NewPos = FMath::Lerp(SelectedCameraSpline->ReferencePoints[index].CameraPosition,
								 SelectedCameraSpline->ReferencePoints[index + 1].CameraPosition, Value);
	FRotator NewRot = FMath::Lerp(SelectedCameraSpline->ReferencePoints[index].CameraRotation,
								  SelectedCameraSpline->ReferencePoints[index + 1].CameraRotation, Value);

	SetViewportPosition(NewPos);
	SetViewportRotation(NewRot);

	// DynamicSplineCameraDebugUtils::Print(FString::SanitizeFloat(value), FColor::Green);
}

#pragma endregion

#pragma region Config Data

void USplineCameraWidget::SaveConfigData()
{
	// Save Overlap Filter
	TArray<FString> ClassNames;

	for (UClass* Class : OverlapFilter)
	{
		if (Class)
		{
			ClassNames.Add(Class->GetPathName());
		}
	}

	FString ConfigFilePath = FPaths::ProjectConfigDir() / TEXT("DynamicSplineCamera.ini");
	GConfig->SetArray(TEXT("TriggerBox"), TEXT("OverlapFilter"), ClassNames, ConfigFilePath);

	// Save bActivateOnlyIfActorCenterIsInsideBox
	GConfig->SetBool(TEXT("TriggerBox"), TEXT("bActivateOnlyIfActorCenterIsInsideBox"), bActivateOnlyIfActorCenterIsInsideBox, ConfigFilePath);
}

void USplineCameraWidget::LoadConfigData()
{
	// Load Overlap Filter
	TArray<FString> LoadedClassNames;
	FString ConfigFilePath = FPaths::ProjectConfigDir() / TEXT("DynamicSplineCamera.ini");

	GConfig->GetArray(TEXT("TriggerBox"), TEXT("OverlapFilter"), LoadedClassNames, ConfigFilePath);

	OverlapFilter.Empty();
	for (const FString& ClassName : LoadedClassNames)
	{
		UClass* LoadedClass = StaticLoadClass(UObject::StaticClass(), nullptr, *ClassName);
		if (LoadedClass)
		{
			OverlapFilter.Add(LoadedClass);
		}
	}

	// Load bActivateOnlyIfActorCenterIsInsideBox
	GConfig->GetBool(TEXT("TriggerBox"), TEXT("bActivateOnlyIfActorCenterIsInsideBox"), bActivateOnlyIfActorCenterIsInsideBox, ConfigFilePath);
}
#pragma endregion

#pragma region Viewport
void USplineCameraWidget::SetViewportPosition(const FVector& Position)
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

void USplineCameraWidget::SetViewportRotation(const FRotator& Rotation)
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

bool USplineCameraWidget::RaycastViewport(FHitResult& HitResult)
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

void USplineCameraWidget::OverrideCamera()
{
	if (!SelectedCameraSpline || !SelectedReferencePointActor) return ;

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
		return;
	}

	SelectedCameraSpline->UpdateReferencePointsData();
	RefreshWidgetData();
}

#pragma endregion

// void USplineCameraWidget::CreateReferencePointArrayFromSplineCamera(ACameraSpline* SplineCamera,
// 	TArray<TSharedPtr<FReferencePoint>>& OutArray)
// {
// 	OutArray.Empty();
// 	for (FReferencePoint ReferencePoint : SplineCamera->ReferencePoints)
// 	{
// 		FReferencePoint* NewRef = new FReferencePoint();
//
// 		NewRef->CameraPosition = ReferencePoint.CameraPosition;
// 		NewRef->CameraRotation = ReferencePoint.CameraRotation;
// 		NewRef->PositionActor = ReferencePoint.PositionActor;
//
// 		OutArray.Add(MakeShareable(NewRef));
// 	}
// }
