// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineCameraActor.h"

#include "CameraSpline.h"
#include "Kismet/KismetMathLibrary.h"

ASplineCameraActor::ASplineCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASplineCameraActor::SetCameraPosition(float DeltaSeconds)
{
	if (SplineReference && ActiveCameraSpline)
	{
		FVector CameraPosition;
		FRotator CameraRotation;
		ActiveCameraSpline->SetCameraTransformAtPosition(SplineReference->GetActorLocation(), CameraPosition,
		                                                 CameraRotation, bUseClosestLineToReference);
		
		int SplineCameraIndex = ActiveCameraSpline->GetCurrentCameraIndex();
		float CameraSpeedLerpValue = FMath::Clamp(DeltaSeconds * CameraSpeed, 0.0f, 1.0f);

		if (!bForceFollowOnSpline || CurrentCameraIndex == SplineCameraIndex)
		{
			FRotator LookAtReferenceRotation = UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation(), SplineReference->GetActorLocation());
			CameraRotation = FMath::Lerp(CameraRotation, LookAtReferenceRotation, LookAtReferenceWeight);

			FVector FinalCameraPosition = FMath::Lerp(GetActorLocation(), CameraPosition, CameraSpeedLerpValue);
			FRotator FinalCameraRotation = FMath::Lerp(GetActorRotation(), CameraRotation, CameraSpeedLerpValue);

			SetActorLocation(FinalCameraPosition);
			SetActorRotation(FinalCameraRotation);

			CurrentCameraIndex = ActiveCameraSpline->GetCurrentCameraIndex();
		}
		else
		{
			// Calculamos la distancia hacia el punto más cercano de la referencia según el índice de la spline
			float TotalDistance = 0.0f;
			if (SplineCameraIndex > CurrentCameraIndex)
			{
				// bForceFollowOnSpline = false;

				// UE_LOG(LogTemp, Warning, TEXT("=================================================="));

				// Tenemos que avanzar hacia adeltante
				float DistanceToNextPoint = FVector::Dist(GetActorLocation(),
				                                          ActiveCameraSpline->SplinePoints[CurrentCameraIndex + 1].
				                                          CameraPosition);
				TotalDistance = TotalDistance + DistanceToNextPoint;

				for (int i = CurrentCameraIndex + 1; i < SplineCameraIndex; i++)
				{
					DistanceToNextPoint = FVector::Dist(ActiveCameraSpline->SplinePoints[i].CameraPosition,
					                                    ActiveCameraSpline->SplinePoints[i + 1].CameraPosition);
					TotalDistance = TotalDistance + DistanceToNextPoint;
				}

				float ClosestPointToReferenceDistance = ActiveCameraSpline->GetDistanceFromCurrentSplinePoint(
					SplineReference->GetActorLocation());

				float DistanceRatio = ClosestPointToReferenceDistance / (FVector::Dist(
					ActiveCameraSpline->SplinePoints[SplineCameraIndex].Position,
					ActiveCameraSpline->SplinePoints[SplineCameraIndex + 1].Position));

				float CameraDistance = FVector::Dist(ActiveCameraSpline->SplinePoints[SplineCameraIndex].CameraPosition,
				                                     ActiveCameraSpline->SplinePoints[SplineCameraIndex + 1].
				                                     CameraPosition) * DistanceRatio;

				TotalDistance = TotalDistance + CameraDistance;

				// UE_LOG(LogTemp, Warning, TEXT("Distance Ratio del inicio: %f"), DistanceRatio);
				// UE_LOG(LogTemp, Warning, TEXT("Distance to Spline Point: %f"), DistanceToSplinePointIndex);

				// Según el valor de interpolación sacado por la velocidad de la cámara, obtenemos la distancia que debemos recorrer en la spline
				float DistanceToTravel = TotalDistance * CameraSpeedLerpValue;

				// UE_LOG(LogTemp, Warning, TEXT("Distance to travel: %f"), DistanceToTravel);

				// Recorremos la spline hasta que nos pasemos
				float Distance = 0;
				int SelectedIndex = CurrentCameraIndex;

				DistanceToNextPoint = FVector::Dist(GetActorLocation(),
				                                    ActiveCameraSpline->SplinePoints[CurrentCameraIndex + 1].
				                                    CameraPosition);
				Distance = Distance + DistanceToNextPoint;

				// UE_LOG(LogTemp, Warning, TEXT("----- Entro en la condición ------"));
				if (Distance < DistanceToTravel)
				{
					for (int i = CurrentCameraIndex + 1; true; i++)
					{
						// UE_LOG(LogTemp, Warning, TEXT("bucle: %i"), i);

						DistanceToNextPoint = FVector::Dist(ActiveCameraSpline->SplinePoints[i].CameraPosition,
						                                    ActiveCameraSpline->SplinePoints[i + 1].CameraPosition);

						Distance = Distance + DistanceToNextPoint;

						if (Distance >= DistanceToTravel)
						{
							// Hacemos la interpolación aquí
							SelectedIndex = i;
							// UE_LOG(LogTemp, Warning, TEXT("----- Salgo del bucle ------"));
							break;
						}
					}
				}

				// UE_LOG(LogTemp, Warning, TEXT("Distancia a la que nos hemos pasado: %f"), Distance);

				float DistanceToReverse = Distance - DistanceToTravel;

				// UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
				// UE_LOG(LogTemp, Warning, TEXT("DistanceToTravel: %f"), DistanceToTravel);

				// UE_LOG(LogTemp, Warning, TEXT("Distance to reverse: %f"), DistanceToReverse);

				DistanceToNextPoint = FVector::Dist(ActiveCameraSpline->SplinePoints[SelectedIndex].CameraPosition,
				                                    ActiveCameraSpline->SplinePoints[SelectedIndex + 1].
				                                    CameraPosition);

				// UE_LOG(LogTemp, Warning, TEXT("Ultima distancia entre nuestro punto al siguiente: %f"),
				//        DistanceToNextPoint);

				float ForwardDistance = DistanceToNextPoint - DistanceToReverse;

				// UE_LOG(LogTemp, Warning, TEXT("ForwardDistance: %f"), ForwardDistance);

				float Ratio = ForwardDistance / DistanceToNextPoint;

				// UE_LOG(LogTemp, Warning, TEXT("Ratio: %f"), Ratio);

				FVector FinalCameraPosition = FMath::Lerp(
					ActiveCameraSpline->SplinePoints[SelectedIndex].CameraPosition,
					ActiveCameraSpline->SplinePoints[SelectedIndex + 1].CameraPosition, Ratio);
				FRotator FinalCameraRotation = FMath::Lerp(
					ActiveCameraSpline->SplinePoints[SelectedIndex].CameraRotation,
					ActiveCameraSpline->SplinePoints[SelectedIndex + 1].CameraRotation, Ratio);

				FRotator LookAtReferenceRotation = UKismetMathLibrary::FindLookAtRotation(
					GetActorLocation(), SplineReference->GetActorLocation());
				FinalCameraRotation = FMath::Lerp(FinalCameraRotation, LookAtReferenceRotation, LookAtReferenceWeight);

				SetActorLocation(FinalCameraPosition);
				SetActorRotation(FinalCameraRotation);

				CurrentCameraIndex = SelectedIndex;
			}
			else /*if (false)*/
			{
				// Caso de ir hacia atrás

				// bForceFollowOnSpline = false;

				// UE_LOG(LogTemp, Warning, TEXT("=================================================="));

				float DistanceToNextPoint = FVector::Dist(GetActorLocation(),
				                                          ActiveCameraSpline->SplinePoints[CurrentCameraIndex].
				                                          CameraPosition);
				TotalDistance = TotalDistance + DistanceToNextPoint;
				// UE_LOG(LogTemp, Warning, TEXT("TotalDistance 1: %f"), TotalDistance);
				for (int i = SplineCameraIndex + 1; i < CurrentCameraIndex; i++)
				{
					DistanceToNextPoint = FVector::Dist(ActiveCameraSpline->SplinePoints[i].CameraPosition,
					                                    ActiveCameraSpline->SplinePoints[i + 1].CameraPosition);
					TotalDistance = TotalDistance + DistanceToNextPoint;
					// UE_LOG(LogTemp, Warning, TEXT("TotalDistance 2: %f"), TotalDistance);
				}

				float ClosestPointToReferenceDistance = ActiveCameraSpline->GetDistanceFromCurrentSplinePoint(
					SplineReference->GetActorLocation());

				float DistanceRatio = ClosestPointToReferenceDistance / (FVector::Dist(
					ActiveCameraSpline->SplinePoints[SplineCameraIndex].Position,
					ActiveCameraSpline->SplinePoints[SplineCameraIndex + 1].Position));

				float CameraDistance = FVector::Dist(ActiveCameraSpline->SplinePoints[SplineCameraIndex].CameraPosition,
				                                     ActiveCameraSpline->SplinePoints[SplineCameraIndex + 1].
				                                     CameraPosition) * DistanceRatio;

				// 1 - x
				CameraDistance = FVector::Dist(ActiveCameraSpline->SplinePoints[SplineCameraIndex].CameraPosition,
				                               ActiveCameraSpline->SplinePoints[SplineCameraIndex + 1].
				                               CameraPosition) - CameraDistance;

				TotalDistance = TotalDistance + CameraDistance;

				// UE_LOG(LogTemp, Warning, TEXT("TotalDistance 3: %f"), TotalDistance);
				// UE_LOG(LogTemp, Warning, TEXT("Distance to Spline Point: %f"), DistanceToSplinePointIndex);

				// Según el valor de interpolación sacado por la velocidad de la cámara, obtenemos la distancia que debemos recorrer en la spline
				float DistanceToTravel = TotalDistance * CameraSpeedLerpValue;

				// UE_LOG(LogTemp, Warning, TEXT("Distance to travel: %f"), DistanceToTravel);

				// Recorremos la spline hasta que nos pasemos
				float Distance = 0;
				int SelectedIndex = CurrentCameraIndex;

				DistanceToNextPoint = FVector::Dist(GetActorLocation(),
				                                    ActiveCameraSpline->SplinePoints[CurrentCameraIndex].
				                                    CameraPosition);
				Distance = Distance + DistanceToNextPoint;

				// UE_LOG(LogTemp, Warning, TEXT("----- Entro en la condición ------"));
				if (Distance < DistanceToTravel)
				{
					for (int i = CurrentCameraIndex - 1; i >= SplineCameraIndex; i--)
					{
						// UE_LOG(LogTemp, Warning, TEXT("bucle: %i"), i);

						DistanceToNextPoint = FVector::Dist(ActiveCameraSpline->SplinePoints[i].CameraPosition,
						                                    ActiveCameraSpline->SplinePoints[i + 1].CameraPosition);

						Distance = Distance + DistanceToNextPoint;

						if (Distance >= DistanceToTravel)
						{
							// Hacemos la interpolación aquí
							SelectedIndex = i;
							// UE_LOG(LogTemp, Warning, TEXT("----- Salgo del bucle ------"));
							break;
						}
					}
				}

				// UE_LOG(LogTemp, Warning, TEXT("Distancia a la que nos hemos pasado: %f"), Distance);

				float DistanceToReverse = Distance - DistanceToTravel;

				// UE_LOG(LogTemp, Warning, TEXT("Distance to reverse: %f"), DistanceToReverse);

				DistanceToNextPoint = FVector::Dist(ActiveCameraSpline->SplinePoints[SelectedIndex].CameraPosition,
				                                    ActiveCameraSpline->SplinePoints[SelectedIndex + 1].
				                                    CameraPosition);

				// UE_LOG(LogTemp, Warning, TEXT("Ultima distancia entre nuestro punto al siguiente: %f"),
				//        DistanceToNextPoint);

				// float ForwardDistance = DistanceToNextPoint - DistanceToReverse;

				// UE_LOG(LogTemp, Warning, TEXT("ForwardDistance: %f"), ForwardDistance);

				float Ratio = (DistanceToReverse / DistanceToNextPoint);

				// UE_LOG(LogTemp, Warning, TEXT("Ratio: %f"), Ratio);

				FVector FinalCameraPosition = FMath::Lerp(
					ActiveCameraSpline->SplinePoints[SelectedIndex].CameraPosition,
					ActiveCameraSpline->SplinePoints[SelectedIndex + 1].CameraPosition, Ratio);
				FRotator FinalCameraRotation = FMath::Lerp(
					ActiveCameraSpline->SplinePoints[SelectedIndex].CameraRotation,
					ActiveCameraSpline->SplinePoints[SelectedIndex + 1].CameraRotation, Ratio);

				FRotator LookAtReferenceRotation = UKismetMathLibrary::FindLookAtRotation(
					GetActorLocation(), SplineReference->GetActorLocation());
				FinalCameraRotation = FMath::Lerp(FinalCameraRotation, LookAtReferenceRotation, LookAtReferenceWeight);

				SetActorLocation(FinalCameraPosition);
				SetActorRotation(FinalCameraRotation);

				CurrentCameraIndex = SelectedIndex;

				// DrawDebugSphere(GetWorld(), ActiveCameraSpline->SplinePoints[SelectedIndex].CameraPosition, 50.0f, 10,
				//                 FColor::Blue);
				// DrawDebugSphere(GetWorld(), FinalCameraPosition, 50.0f, 10, FColor::Yellow);
			}
		}
	}
}

void ASplineCameraActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetCameraPosition(DeltaSeconds);
}

void ASplineCameraActor::SetActiveSpline(ACameraSpline* Spline)
{
	ActiveCameraSpline = Spline;
	ActiveCameraSpline->UpdateIndexFromNewReference(SplineReference->GetActorLocation());
	SetCameraIndex(ActiveCameraSpline->GetCurrentCameraIndex());
}
