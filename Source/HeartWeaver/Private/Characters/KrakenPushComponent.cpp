// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenPushComponent.h"

#include "Actor/KrakenPushableActor.h"
#include "Characters/KrakenCharacter.h"
#include "Characters/KrakenCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UKrakenPushComponent::UKrakenPushComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	UActorComponent::SetComponentTickEnabled(false);
}

void UKrakenPushComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AKrakenCharacter>(GetOwner());
}

void UKrakenPushComponent::BeginPush(AKrakenPushableActor* PushableObject)
{
	if (CurrentPushableObject == nullptr)
	{
		CurrentPushableObject = PushableObject;
		Character->AttachToActor(Cast<AActor>(CurrentPushableObject), FAttachmentTransformRules::KeepWorldTransform);
		Character->GetKrakenCharacterMovementComponent()->bOrientRotationToMovement = false;
		UActorComponent::SetComponentTickEnabled(true);
	}
}

void UKrakenPushComponent::EndPush()
{
	if (CurrentPushableObject != nullptr)
	{
		CurrentPushableObject = nullptr;
		Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Character->GetKrakenCharacterMovementComponent()->bOrientRotationToMovement = true;
		UActorComponent::SetComponentTickEnabled(false);
		IsMovingAnObject = false;
	}
}

bool UKrakenPushComponent::MakeCapsuleTrace(const float Radius, const float HalfHeight) const
{
	FHitResult HitResult;
	const bool DidItHitSomething = UKismetSystemLibrary::CapsuleTraceSingle(
		Character,
		Character->GetActorLocation(),
		Character->GetActorLocation(),
		Radius,
		HalfHeight,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	return DidItHitSomething;
}

void UKrakenPushComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentPushableObject)
	{
		return;
	}

	CurrentActorZRotation = Character->GetActorRotation().Yaw;

	if (!CanMoveRight)
	{
		RightMove = UKismetMathLibrary::Clamp(RightMove, 0.0f, 1.0f);
	}
	if (!CanMoveLeft)
	{
		RightMove = UKismetMathLibrary::Clamp(RightMove, -1.0f, 0.0f);
	}
	if (!CanMoveUp)
	{
		ForwardMove = UKismetMathLibrary::Clamp(ForwardMove, 0.0f, 1.0f);
	}
	if (!CanMoveDown)
	{
		ForwardMove = UKismetMathLibrary::Clamp(ForwardMove, -1.0f, 0.0f);
	}
	MoveCurrentPushableObject(DeltaTime);
}

void UKrakenPushComponent::CheckBackwardsCharacterCollision(int ChosenDirection)
{
	if (MakeCapsuleTrace(Character->GetCapsuleComponent()->GetScaledCapsuleRadius(), Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()))
	{
		switch (ChosenDirection)
		{
		case 0:
			CanMoveUp = false;
			break;
		case 1:
			CanMoveRight = false;
			break;
		case 2:
			CanMoveDown = false;
			break;
		case 3:
			CanMoveLeft = false;
			break;
		default: break;
		}
	}
	else
	{
		CanMoveUp = true;
		CanMoveRight = true;
		CanMoveDown = true;
		CanMoveLeft = true;
	}
}

void UKrakenPushComponent::StopMovement()
{
	ForwardMove = 0.0f;
	RightMove = 0.0f;
}

bool UKrakenPushComponent::MakeLineTraceToSide(const bool CheckRight) const
{
	FHitResult HitResult;
	const FRotator CharacterRotation = Character->GetActorRotation();
	const bool FoundHit = UKismetSystemLibrary::LineTraceSingle(
		Character,
		Character->GetActorLocation(),
		Character->GetActorLocation() + ((CheckRight ? Character->GetActorRightVector() : -Character->GetActorRightVector()) * 60.0),
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	return FoundHit;
}

void UKrakenPushComponent::MoveCurrentPushableObject(float DeltaTime)
{
	FVector DeltaLocation = FVector::ZeroVector;
	int CharacterZRotationIndex = -1;
	if (CurrentActorZRotation >= 0.0f and CurrentActorZRotation < 80.0f)
	{
		CharacterZRotationIndex = 0;
	}
	else if (CurrentActorZRotation >= 80.0f and CurrentActorZRotation < 170.0f)
	{
		CharacterZRotationIndex = 1;
	}
	else if (CurrentActorZRotation >= 170.0f and CurrentActorZRotation < 260.0f)
	{
		CharacterZRotationIndex = 2;
	}
	else if (CurrentActorZRotation >= -90.0f and CurrentActorZRotation < 0.0f)
	{
		CharacterZRotationIndex = 3;
	}


	PushVelocity = FVector::ZeroVector;
	switch (CharacterZRotationIndex)
	{
	case 0:
		DeltaLocation = GetDeltaLocation(DeltaTime, ForwardMove, RightMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			PushVelocity = FVector(PushSpeed, 0.f, 0.f);
		}
		break;
	case 1:
		DeltaLocation = GetDeltaLocation(DeltaTime, RightMove, -ForwardMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			PushVelocity = FVector(0.f, PushSpeed, 0.f);
		}
		break;
	case 2:
		DeltaLocation = GetDeltaLocation(DeltaTime, -ForwardMove, -RightMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			PushVelocity = FVector(-PushSpeed, 0.f, 0.f);
		}
		break;
	case 3:
		DeltaLocation = GetDeltaLocation(DeltaTime, -RightMove, ForwardMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			PushVelocity = FVector(0.f, -PushSpeed, 0.f);
		}
		break;
	default: break;
	}


	if (DeltaLocation != FVector::ZeroVector)
	{
		CurrentPushableObject->AddActorWorldOffset(DeltaLocation, true);
		StopMovement();
		CheckBackwardsCharacterCollision(CharacterZRotationIndex);
		if (MakeLineTraceToSide())
		{
			switch (CharacterZRotationIndex)
			{
			case 0: CanMoveRight = false;
				break;
			case 1: CanMoveUp = false;
				break;
			case 2: CanMoveLeft = false;
				break;
			case 3: CanMoveDown = false;
				break;
			default: break;
			}
		}
		if (MakeLineTraceToSide(true))
		{
			switch (CharacterZRotationIndex)
			{
			case 0: CanMoveLeft = false;
				break;
			case 1: CanMoveDown = false;
				break;
			case 2: CanMoveRight = false;
				break;
			case 3: CanMoveUp = false;
				break;
			default: break;
			}
		}
	}

	PushRotation = UKismetMathLibrary::MakeRotFromX(DeltaLocation);
}

FVector UKrakenPushComponent::GetDeltaLocation(float DeltaSecond, float InForwardMove, float InRightMove)
{
	if (InForwardMove != 0.0f or InRightMove != 0.0f)
	{
		IsMovingAnObject = true;
		const FVector ForwardDirection = Character->GetActorForwardVector();
		const FVector RightDirection = Character->GetActorRightVector();
		const FVector DeltaLocation = (ForwardDirection * InForwardMove + RightDirection * InRightMove) * PushSpeed * DeltaSecond;

		return DeltaLocation;
	}

	return FVector::ZeroVector;
}
