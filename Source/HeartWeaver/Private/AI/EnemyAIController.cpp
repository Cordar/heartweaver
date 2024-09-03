// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/EnemyAIController.h"

#include "AI/KrakenNavBox.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (!ControlledCharacter)
	{
		return;
	}
	CharacterMovement = Cast<UCharacterMovementComponent>(ControlledCharacter->GetMovementComponent());
	if (!CharacterMovement)
	{
		return;
	}

	ObtainNavMeshBox();

	if (NavMeshBox)
	{
		ControlledCharacter->SetActorLocation(
			GetCharacterLocationToNavMeshPoint(
				NavMeshBox->GetClosestPointInNavMesh(ControlledCharacter->GetActorLocation())));
		Debug_GenerateRandomPath();
	}
}

void AEnemyAIController::HandlePath(float DeltaSeconds)
{
	if (CurrentPath.Num() > 0)
	{
		int ClosestPointIndex = GetClosestPointInArray(ControlledCharacter->GetActorLocation(), CurrentPath);
		if (ClosestPointIndex < CurrentPath.Num() - 1)
		{
			int NextPointIndex = ClosestPointIndex + 1;

			FVector NextPoint = GetCharacterLocationToNavMeshPoint(CurrentPath[NextPointIndex]);

			FVector Direction = NextPoint - ControlledCharacter->GetActorLocation();
			Direction.Normalize();

			// CharacterMovement->AddInputVector(FVector(1.0f, 0.0f, 0.0f));
			CharacterMovement->AddInputVector(Direction);
			// MoveToLocation(NextPoint);
		}
		else
		{
			Debug_GenerateRandomPath();
		}
	}
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentPath.Num() > 0)
	{
		HandlePath(DeltaSeconds);
		Debug_DebugPath();
	}
}


void AEnemyAIController::ObtainNavMeshBox()
{
	TArray<AActor*> Boxes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKrakenNavBox::StaticClass(), Boxes);

	if (Boxes.Num() > 0)
	{
		AKrakenNavBox* SelectedBox = nullptr;

		float ClosestDistance = 9999999.0f;
		for (int i = 0; i < Boxes.Num(); i++)
		{
			AKrakenNavBox* Box = Cast<AKrakenNavBox>(Boxes[i]);
			FVector ClosestPoint = Box->GetClosestPointInNavMesh(ControlledCharacter->GetActorLocation());
			float Distance = FVector::Dist(ClosestPoint, ControlledCharacter->GetActorLocation());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				SelectedBox = Box;
			}
		}

		if (SelectedBox)
		{
			NavMeshBox = SelectedBox;

			if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(
				ControlledCharacter->GetMovementComponent()))
			{
				FVector GravityDirection = FVector(0.0f, 0.0f, 0.0f) - NavMeshBox->GetActorUpVector();
				MovementComponent->SetGravityDirection(GravityDirection);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NavMesh Box no encontrado"));
	}
}


FVector AEnemyAIController::GetCharacterLocationToNavMeshPoint(FVector NavMeshPoint) const
{
	UCapsuleComponent* Capsule = ControlledCharacter->GetCapsuleComponent();
	FVector NewLocalization = NavMeshPoint + (NavMeshBox->GetActorUpVector()) * Capsule->GetScaledCapsuleHalfHeight();
	return NewLocalization;
}

int AEnemyAIController::GetClosestPointInArray(FVector Point, TArray<FVector> Array)
{
	float ClosestDistance = UE_BIG_NUMBER;
	int SelectedIndex = 0;
	for (int i = 0; i < Array.Num(); i++)
	{
		float Distance = FVector::Dist(Point, Array[i]);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			SelectedIndex = i;
		}
	}
	return SelectedIndex;
}

void AEnemyAIController::Debug_GenerateRandomPath()
{
	FVector InitialPoint = NavMeshBox->GetClosestPointInNavMesh(ControlledCharacter->GetActorLocation());
	FVector RandPoint = NavMeshBox->GetRandomPointInNavMesh();
	CurrentPath = NavMeshBox->GetPath(InitialPoint, RandPoint);
}

void AEnemyAIController::Debug_DebugPath()
{
	UWorld* World = GetWorld();
	for (int i = 0; i < CurrentPath.Num() - 1; i++)
	{
		DrawDebugLine(World, CurrentPath[i], CurrentPath[i + 1], FColor::Magenta);
	}
}
