// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraSpline.h"

#include "CameraSplinePointReference.h"
#include "Components/BillboardComponent.h"
#include "Components/LineBatchComponent.h"

// Sets default values
ACameraSpline::ACameraSpline()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root component"));
	// Root->SetupAttachment(GetRootComponent());
	SetRootComponent(Root);

	LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("Camera Line Batch"));
	LineBatchComponent->SetupAttachment(GetRootComponent());

#if WITH_EDITORONLY_DATA

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(GetRootComponent());

#endif
}

#if WITH_EDITOR

void ACameraSpline::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateReferencePointsData();
}


void ACameraSpline::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (GEngine)
	{
		if (UEditorEngine* editor = CastChecked<UEditorEngine>(GEngine))
		{
			if (!editor->bIsSimulatingInEditor && GetWorld()->WorldType == EWorldType::Editor)
			{
				ReferencePoints.Empty();
			}
		}
	}
}


#endif

// Called when the game starts or when spawned
void ACameraSpline::BeginPlay()
{
	Super::BeginPlay();

	// UpdateReferencePointsData();
}

void ACameraSpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateReferencePointsData();
}

// Called every frame
void ACameraSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACameraSpline::GetCameraTransformAtPosition(const FVector& Point, FVector& CameraLocation,
                                                 FRotator& CameraRotation)
{
	SetCameraIndexAtPosition(Point);

	const FVector Pos = GetClosestPointBetweenSplinePoints(Point, CameraSplineIndex);

	// Dada la posición, sacamos el valor de interpolación en relación a la distancia
	float Distance = FVector::Distance(SplinePoints[CameraSplineIndex].Position, Pos);
	float MaxDistance = FVector::Distance(SplinePoints[CameraSplineIndex].Position,
	                                      SplinePoints[CameraSplineIndex + 1].Position);

	float LerpValue = Distance / MaxDistance;

	// UE_LOG(LogTemp, Warning, TEXT("%f"), LerpValue);


	CameraLocation = FMath::Lerp(SplinePoints[CameraSplineIndex].CameraPosition,
	                             SplinePoints[CameraSplineIndex + 1].CameraPosition, LerpValue);

	CameraRotation = FMath::Lerp(SplinePoints[CameraSplineIndex].CameraRotation,
	                             SplinePoints[CameraSplineIndex + 1].CameraRotation, LerpValue);
}

FVector ACameraSpline::GetClosestPointBetweenSplinePoints(FVector Position, int PointIndex)
{
	if (PointIndex >= SplinePoints.Num() - 1)
	{
		return SplinePoints[SplinePoints.Num() - 1].Position;
	}

	FVector InitialPoint = SplinePoints[PointIndex].Position;
	FVector NextPoint = SplinePoints[PointIndex + 1].Position;

	return FMath::ClosestPointOnSegment(Position, InitialPoint, NextPoint);
}

float ACameraSpline::GetDistanceFromCurrentSplinePoint(const FVector& Position)
{
	FVector PositionInSegment = GetClosestPointBetweenSplinePoints(Position, CameraSplineIndex);

	return FVector::Dist(SplinePoints[CameraSplineIndex].Position, PositionInSegment);
}

void ACameraSpline::UpdateReferencePointsData()
{
	UpdateReferencePoints();
	CreateSplinePoints(CurveSubdivision);
	DrawDebugLines();
}

void ACameraSpline::UpdateIndexFromNewReference(FVector Position)
{
	float ClosestDistance = 9999999999999.0f;
	int SelectedIndex = 0;
	bool bIsInsideTheSpline = false;
	for (int i = 0; i < SplinePoints.Num() - 1; i++)
	{
		FVector DirRef0ToPoint = Position - SplinePoints[i].Position;
		DirRef0ToPoint.Normalize();

		FVector DirRef1ToPoint = Position - SplinePoints[i + 1].Position;
		DirRef1ToPoint.Normalize();

		double Dot0 = FVector::DotProduct(SplinePoints[i].DirectionFwrd, DirRef0ToPoint);
		double Dot1 = FVector::DotProduct(-SplinePoints[i + 1].DirectionFwrd, DirRef1ToPoint);

		if (Dot0 > 0 && Dot1 > 0)
		{
			// Pertenecemos a este plano

			float Distance = FVector::Dist(
				FMath::ClosestPointOnSegment(Position, SplinePoints[i].Position, SplinePoints[i + 1].Position),
				Position);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				SelectedIndex = i;
				bIsInsideTheSpline = true;
			}
		}
	}

	if (!bIsInsideTheSpline)
	{
		// Comprobamos por si acaso, si está mas cerca el punto de inicio o el punto final en caso de no formar parte de la spline
		float DistanceFirst = FVector::Dist(Position, SplinePoints[0].Position);
		float DistanceLast = FVector::Dist(Position, SplinePoints[SplinePoints.Num() - 1].Position);

		if (DistanceFirst > DistanceLast)
			SelectedIndex = SplinePoints.Num() - 2;
	}

	CameraSplineIndex = SelectedIndex;
}

#if WITH_EDITOR

void ACameraSpline::ReestructurateArrayFromDuplicatedReferencePointActor(ACameraSplinePointReference* DuplicatedPoint)
{
	if (GIsPlayInEditorWorld)
	{
		return;
	}

	if (DuplicatedPoint->ReferenceIndex >= 0)
	{
		// Aumentamos su índice y reordenamos el array de referencias
		TArray<FReferencePoint> NewArray;
		for (int i = 0; i < ReferencePoints.Num(); i++)
		{
			NewArray.Add(ReferencePoints[i]);
			if (i == DuplicatedPoint->ReferenceIndex)
			{
				FReferencePoint NewReferencePointData = FReferencePoint(ReferencePoints[i]);
				NewReferencePointData.PositionActor = DuplicatedPoint;

				if (i > 0)
				{
					// Lo desplazamos 100 centímetros en función del anterior punto
					FVector Pos1 = ReferencePoints[i].
					               PositionActor->GetActorLocation();
					FVector Pos2 = ReferencePoints[i - 1].
					               PositionActor->GetActorLocation();
					FVector Dir = Pos1 - Pos2;
					Dir.Normalize();

					FVector NewPosition = Pos1 + (Dir * 100.0f);
					NewReferencePointData.PositionActor->SetActorLocation(NewPosition);
				}
				else if (ReferencePoints.Num() > 1)
				{
					// Lo desplazamos 100 centímetros en función del punto posterior
					FVector Pos1 = ReferencePoints[i].
					               PositionActor->GetActorLocation();
					FVector Pos2 = ReferencePoints[i + 1].
					               PositionActor->GetActorLocation();
					FVector Dir = Pos2 - Pos1;
					Dir.Normalize();

					FVector NewPosition = Pos1 + (Dir * 100.0f);
					NewReferencePointData.PositionActor->SetActorLocation(NewPosition);
				}

				NewArray.Add(NewReferencePointData);
			}
		}
		ReferencePoints = NewArray;

		UpdateReferencePointsData();
	}
	else
	{
		// Puesto que no tiene asignado ningún índice lo ponemos al final
		FReferencePoint NewReferencePointData = FReferencePoint(ReferencePoints[ReferencePoints.Num() - 1]);
		NewReferencePointData.PositionActor = DuplicatedPoint;

		ReferencePoints.Add(NewReferencePointData);

		UpdateReferencePointsData();
	}
}
#endif

void ACameraSpline::DrawDebugLines()
{
	FlushPersistentDebugLines(GetWorld());
	LineBatchComponent->Flush();

	for (int i = 0; i < SplinePoints.Num() - 1; i++)
	{
		if (bDrawReferencePointsLine)
		{
			LineBatchComponent->DrawLine(SplinePoints[i].Position,
			                             SplinePoints[i + 1].Position,
			                             FLinearColor::Yellow,
			                             0, 1.5f, 999999999.9f);
		}

		if (bDrawCameraLine)
		{
			LineBatchComponent->DrawLine(SplinePoints[i].CameraPosition,
			                             SplinePoints[i + 1].CameraPosition,
			                             FLinearColor::Green,
			                             0, 1.5f, 999999999.9f);
		}
	}

	if (bDrawCameraLook)
	{
		for (int i = 0; i < ReferencePoints.Num(); i++)
		{
			LineBatchComponent->DrawLine(ReferencePoints[i].CameraPosition,
			                             ReferencePoints[i].CameraPosition + (ReferencePoints[i].CameraRotation.Vector()
				                             *
				                             70.0f),
			                             FLinearColor::Blue,
			                             0, 1, 999999999.9f);
		}
	}
}

void ACameraSpline::UpdateReferencePoints()
{
	for (int i = 0; i < ReferencePoints.Num(); i++)
	{
		if (!ReferencePoints[i].PositionActor || ReferencePoints[i].PositionActor->IsActorBeingDestroyed())
		{
			ReferencePoints.RemoveAt(i);
			i--;
			continue;
		}
		ReferencePoints[i].PositionActor->ReferenceIndex = i;
	}
}

#pragma region Spline Creation

void ACameraSpline::CreateSplinePoints(int Subdivision)
{
	SplinePoints.Empty();
	if (ReferencePoints.Num() < 2) return;

	float tIncrement = 1.0f / Subdivision;

	SplinePoints.Reserve(ReferencePoints.Num() * Subdivision + 2);

	// Metemos el primer punto
	SplinePoints.Add(FDynamicSplinePoint(ReferencePoints[0].PositionActor->GetActorLocation(), FVector().Zero(),
	                                     FVector().Zero(),
	                                     ReferencePoints[0].CameraPosition, ReferencePoints[0].CameraRotation));

	for (int i = 1; i < ReferencePoints.Num() - 1; i++)
	{
		FVector PreviousPoint = ReferencePoints[i - 1].PositionActor->GetActorLocation();
		FVector Point = ReferencePoints[i].PositionActor->GetActorLocation();
		FVector NextPoint = ReferencePoints[i + 1].PositionActor->GetActorLocation();

		FVector Dir1 = Point - PreviousPoint;
		FVector Dir2 = Point - NextPoint;

		FVector NewPrevPoint = Point - Dir1 * (CurveAmmount * 1.0f);
		FVector NewNextPoint = Point - Dir2 * (CurveAmmount * 1.0f);

		for (int a = 0; a < Subdivision; a++)
		{
			const float t = a * tIncrement;
			FVector NewPosition = CalculateQuadraticBezierPoint(t, NewPrevPoint, Point, NewNextPoint);

			if (t <= 0.5f)
			{
				float LerpValue = FMath::Lerp(1 - CurveAmmount, 1.0f, (t * 2.0f))
					/*(CurveAmmount) + (1 - CurveAmmount) * (t * 2.0f)*/;
				FVector CamPosition = FMath::Lerp(ReferencePoints[i - 1].CameraPosition,
				                                  ReferencePoints[i].CameraPosition, LerpValue);
				FRotator CamRotation = FMath::Lerp(ReferencePoints[i - 1].CameraRotation,
				                                   ReferencePoints[i].CameraRotation, LerpValue);

				SplinePoints.Add(FDynamicSplinePoint(NewPosition, FVector().Zero(), FVector().Zero(), CamPosition,
				                                     CamRotation));
			}
			else
			{
				float LerpValue = FMath::Lerp(0, CurveAmmount, (t - 0.5f) * 2.0f)
					/*(CurveAmmount) + (1 - CurveAmmount) * (t * 2.0f)*/;
				FVector CamPosition = FMath::Lerp(ReferencePoints[i].CameraPosition,
				                                  ReferencePoints[i + 1].CameraPosition, LerpValue);
				FRotator CamRotation = FMath::Lerp(ReferencePoints[i].CameraRotation,
				                                   ReferencePoints[i + 1].CameraRotation, LerpValue);

				SplinePoints.Add(FDynamicSplinePoint(NewPosition, FVector().Zero(), FVector().Zero(), CamPosition,
				                                     CamRotation));
			}
		}
	}

	// Añadimos el ultimo punto
	SplinePoints.Add(FDynamicSplinePoint(ReferencePoints[ReferencePoints.Num() - 1].PositionActor->GetActorLocation(),
	                                     FVector().Zero(), FVector().Zero(),
	                                     ReferencePoints[ReferencePoints.Num() - 1].CameraPosition,
	                                     ReferencePoints[ReferencePoints.Num() - 1].CameraRotation));


	// A cada punto, le añadimos la dirección para que encontrar el punto más cercano sea más eficiente computacionalmente

	for (int i = 0; i < SplinePoints.Num() - 1; i++)
	{
		FVector DirFwrd = SplinePoints[i + 1].Position - SplinePoints[i].Position;
		DirFwrd.Normalize();
		SplinePoints[i].DirectionFwrd = DirFwrd;

		if (i > 0)
		{
			FVector DirBckrd = SplinePoints[i - 1].Position - SplinePoints[i].Position;
			DirBckrd.Normalize();
			SplinePoints[i].DirectionBckrd = DirBckrd;
		}
	}
}

FVector ACameraSpline::CalculateQuadraticBezierPoint(float t, const FVector& P0, const FVector& P1, const FVector& P2)
{
	float u = 1.0f - t;
	float tt = t * t;
	float uu = u * u;

	FVector point = uu * P0; // (1-t)^2 * P0
	point += 2 * u * t * P1; // 2 * (1-t) * t * P1
	point += tt * P2; // t^2 * P2

	return point;
}


#pragma endregion

void ACameraSpline::SetCameraIndexAtPosition(const FVector& Position)
{
	// Comprobamos primero si está dentro de los planos hechos por nuestros puntos de referencia para saber entre qué puntos estamos
	FVector DirRef0ToPoint = Position - SplinePoints[CameraSplineIndex].Position;
	DirRef0ToPoint.Normalize();

	FVector DirRef1ToPoint = Position - SplinePoints[CameraSplineIndex + 1].Position;
	DirRef1ToPoint.Normalize();

	double Dot0 = FVector::DotProduct(SplinePoints[CameraSplineIndex].DirectionFwrd, DirRef0ToPoint);
	double Dot1 = FVector::DotProduct(-SplinePoints[CameraSplineIndex + 1].DirectionFwrd, DirRef1ToPoint);

	if (Dot0 < 0.00f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("No estamos en el plano correspondiente 0: %f"), Dot0);
		// Decrementamos
		if (CameraSplineIndex > 0)
		{
			CameraSplineIndex--;
			SetCameraIndexAtPosition(Position);
		}
		else
		{
			// No podemos hacer nada más, este es el punto
		}
	}
	else if (Dot1 < 0.00f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("No estamos en el plano correspondiente 1: %f"), Dot1);
		if (CameraSplineIndex < SplinePoints.Num() - 2)
		{
			CameraSplineIndex++;
			SetCameraIndexAtPosition(Position);
		}
		else
		{
			// No podemos hacer nada más, este es el punto
		}
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("ESTAMOS el plano correspondiente"));
	}

	// if (FVector::DotProduct(SplinePoints[CameraIndex].DirectionFwrd, RefPoint0) < 0 || FVector::DotProduct(
	// 	SplinePoints[CameraIndex + 1].DirectionBckrd, RefPoint1) < 0)
	// if (FVector::DotProduct(SplinePoints[CameraIndex].DirectionFwrd, DirRef0ToPoint) < 0 || FVector::DotProduct(SplinePoints[CameraIndex + 1].DirectionBckrd, DirRef1ToPoint) < 0)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No estamos en el plano correspondiente"));
	// 	// Estamos fuera de los planos, hacemos cambio
	// 	if (FVector::Distance(SplinePoints[CameraIndex].Position, Position) < FVector::Distance(
	// 		SplinePoints[CameraIndex + 1].Position, Position))
	// 	{
	// 		// Decrementamos
	// 		if (CameraIndex > 0)
	// 		{
	// 			CameraIndex--;
	// 			SetCameraIndexAtPosition(Position);
	// 		}
	// 		else
	// 		{
	// 			// No podemos hacer nada más, este es el punto
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (CameraIndex < SplinePoints.Num() - 2)
	// 		{
	// 			CameraIndex++;
	// 			SetCameraIndexAtPosition(Position);
	// 		}
	// 		else
	// 		{
	// 			// No podemos hacer nada más, este es el punto
	// 		}
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("ESTAMOS el plano correspondiente"));
	// }
}
