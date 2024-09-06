// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/EnemyAIController.h"

#include "AI/EnemyAIPerception.h"
#include "AI/KrakenNavBox.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/KrakenCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

AEnemyAIController::AEnemyAIController()
{
	// AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}


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

	// NavMesh
	ObtainNavMeshBox();
	if (NavMeshBox)
	{
		ControlledCharacter->SetActorLocation(
			GetCharacterLocationToNavMeshPoint(
				NavMeshBox->GetClosestPointInNavMesh(ControlledCharacter->GetActorLocation())));
		// Debug_GenerateRandomPath();
	}

	// AI Perception
	if (UEnemyAIPerception* AIPerception = Cast<UEnemyAIPerception> (ControlledCharacter->GetComponentByClass(UEnemyAIPerception::StaticClass())))
	{
		// UE_LOG(LogTemp, Warning, TEXT("DETECTAMOS COMPONENTE"));
		AIPerception->OnActorPerceivedDelegate.AddDynamic(this, &AEnemyAIController::OnPerceptActor);
		// AIPerception->OnActorUnperceivedDelegate.AddDynamic(this, &AEnemyAIController::OnUnperceptActor);
	}

	// SetState(InitialState);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController sin Behavior Tree asignado"));
	}
}

void AEnemyAIController::HandlePathMovement(float DeltaSeconds)
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
	}
}

void AEnemyAIController::SetState(EEnemyState NewState)
{
	if (Blackboard)
	{
		EEnemyState PreviousState = static_cast<EEnemyState>(Blackboard.Get()->GetValueAsEnum("State"));
		Blackboard.Get()->SetValueAsEnum("State", static_cast<uint8>(NewState));

		OnStateChange(PreviousState, NewState);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController no tiene Blackboard"));
	}
}

EEnemyState AEnemyAIController::GetState()
{
	if (Blackboard)
	{
		return static_cast<EEnemyState>(Blackboard.Get()->GetValueAsEnum("State"));
	}
	{
		UE_LOG(LogTemp, Error, TEXT("AIController no tiene Blackboard"));
		return EEnemyState::DEACTIVATED;
	}
}

void AEnemyAIController::CreatePathToTarget()
{
	if (Blackboard)
	{
		if (AActor* Target = Cast<AActor>(Blackboard.Get()->GetValueAsObject("Target")))
		{
			FVector InitialPoint = NavMeshBox->GetClosestPointInNavMesh(ControlledCharacter->GetActorLocation());
			FVector FinalPoint = NavMeshBox->GetClosestPointInNavMesh(Target->GetActorLocation());
			CurrentPath = NavMeshBox->GetPath(InitialPoint, FinalPoint);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Target no asignado en la blackboard"));
			CurrentPath = TArray<FVector>();
		}
	}
}

void AEnemyAIController::CreatePathToLocationTarget()
{
	if (Blackboard)
	{
		FVector LocationTarget = Blackboard.Get()->GetValueAsVector("LocationTarget");
		if (LocationTarget.Length() != 0.0f)
		{
			FVector InitialPoint = NavMeshBox->GetClosestPointInNavMesh(ControlledCharacter->GetActorLocation());
			FVector FinalPoint = NavMeshBox->GetClosestPointInNavMesh(LocationTarget);
			CurrentPath = NavMeshBox->GetPath(InitialPoint, FinalPoint);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LocationTarget no asignado en la blackboard o asignado a (0,0,0)"));
			CurrentPath = TArray<FVector>();
		}
	}
}

void AEnemyAIController::SetTarget(AActor* NewTarget)
{
	if (Blackboard)
	{
		Blackboard.Get()->SetValueAsObject("Target", NewTarget);
	}
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	Debug_DebugPath();
#endif
	
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


FVector AEnemyAIController::GetCharacterLocationToNavMeshPoint(const FVector& NavMeshPoint) const
{
	UCapsuleComponent* Capsule = ControlledCharacter->GetCapsuleComponent();
	FVector NewLocalization = NavMeshPoint + (NavMeshBox->GetActorUpVector()) * Capsule->GetScaledCapsuleHalfHeight();
	return NewLocalization;
}

int AEnemyAIController::GetClosestPointInArray(const FVector& Point, TArray<FVector> Array)
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

void AEnemyAIController::OnPerceptActor(AActor* Actor)
{
	if (AKrakenCharacter* Player = Cast<AKrakenCharacter>(Actor))
	{
		SetTarget(Player);
		SetState(EEnemyState::MOVING_TO_TARGET);
	}
}

void AEnemyAIController::OnUnperceptActor(AActor* Actor)
{
	if (Blackboard)
	{
		if (Blackboard.Get()->GetValueAsObject("Target") == Actor)
		{
			Blackboard.Get()->SetValueAsObject("Target", nullptr);
			SetState(EEnemyState::WAITING);
		}
	}
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
		FVector CurrentPoint = CurrentPath[i];
		FVector NextPoint = CurrentPath[i + 1];

		FVector CurrentPointLocal = ControlledCharacter->GetTransform().InverseTransformPosition(CurrentPoint);
		FVector NextPointLocal = ControlledCharacter->GetTransform().InverseTransformPosition(NextPoint);

		CurrentPointLocal.Z = 0.0f;
		NextPointLocal.Z = 0.0f;

		FVector CurrentPointGlobal = ControlledCharacter->GetTransform().TransformPosition(CurrentPointLocal);
		FVector NextPointGlobal = ControlledCharacter->GetTransform().TransformPosition(NextPointLocal);

		DrawDebugLine(World, CurrentPointGlobal, NextPointGlobal, FColor::Magenta);
	}
}
