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
		InitialZLocation = Character->GetActorLocation().Z;
		Character->AttachToActor(Cast<AActor>(CurrentPushableObject), FAttachmentTransformRules::KeepWorldTransform);
		Character->GetKrakenCharacterMovementComponent()->bOrientRotationToMovement = false;
		Character->GetKrakenCharacterMovementComponent()->UnCrouch();
		Character->UnCrouch();
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
		CanMoveDown = true;
		CanMoveUp = true;
		CanMoveLeft = true;
		CanMoveRight = true;
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
	
	if (FMath::Abs(CurrentPushableObject->GetActorLocation().Z - InitialZLocation) >= 100 
		|| FMath::Abs(Character->GetActorLocation().Z - InitialZLocation) >= 100)
	{
		OnPushShouldEnd.Broadcast();
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

bool UKrakenPushComponent::IsCharacterCollidingBackwards() const
{
	return MakeCapsuleTrace(Character->GetCapsuleComponent()->GetScaledCapsuleRadius(), Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
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
	float FinalRotation = CurrentActorZRotation + CurrentPushableObject->ExtraRotation;
	while (FinalRotation > 260.f) FinalRotation -= 360.f;
	while (FinalRotation < -90) FinalRotation += 360.f;
	PushVelocity = FVector::ZeroVector;

	bool IsCollidingBackwards = true, IsCharacterCollidingLeft = true, IsCharacterCollidingRight = true;
	if (FinalRotation >= 0.0f and FinalRotation < 80.0f)
	{
		DeltaLocation = GetDeltaLocation(DeltaTime, ForwardMove, RightMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			CurrentPushableObject->AddActorWorldOffset(DeltaLocation, true);
			IsCollidingBackwards = IsCharacterCollidingBackwards();
			IsCharacterCollidingLeft = MakeLineTraceToSide();
			IsCharacterCollidingRight = MakeLineTraceToSide(true);
			CanMoveUp = !IsCollidingBackwards;
			CanMoveRight = !IsCharacterCollidingLeft;
			CanMoveLeft = !IsCharacterCollidingRight;
		}
	}
	else if (FinalRotation >= 80.0f and FinalRotation < 170.0f)
	{
		DeltaLocation = GetDeltaLocation(DeltaTime, RightMove, -ForwardMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			CurrentPushableObject->AddActorWorldOffset(DeltaLocation, true);
			IsCollidingBackwards = IsCharacterCollidingBackwards();
			IsCharacterCollidingLeft = MakeLineTraceToSide();
			IsCharacterCollidingRight = MakeLineTraceToSide(true);
			CanMoveRight = !IsCollidingBackwards;
			CanMoveDown = !IsCharacterCollidingLeft and !IsCharacterCollidingRight;
		}
	}
	else if (FinalRotation >= 170.0f and FinalRotation < 260.0f)
	{
		DeltaLocation = GetDeltaLocation(DeltaTime, -ForwardMove, -RightMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			CurrentPushableObject->AddActorWorldOffset(DeltaLocation, true);
			IsCollidingBackwards = IsCharacterCollidingBackwards();
			IsCharacterCollidingLeft = MakeLineTraceToSide();
			IsCharacterCollidingRight = MakeLineTraceToSide(true);
			CanMoveDown = !IsCollidingBackwards;
			CanMoveUp = !IsCharacterCollidingLeft;
			CanMoveRight = !IsCharacterCollidingRight;
		}
	}
	else if (FinalRotation >= -90.0f and FinalRotation < 0.0f)
	{
		DeltaLocation = GetDeltaLocation(DeltaTime, -RightMove, ForwardMove);
		if (DeltaLocation != FVector::ZeroVector)
		{
			CurrentPushableObject->AddActorWorldOffset(DeltaLocation, true);
			IsCollidingBackwards = IsCharacterCollidingBackwards();
			IsCharacterCollidingLeft = MakeLineTraceToSide();
			IsCharacterCollidingRight = MakeLineTraceToSide(true);
			CanMoveLeft = !IsCollidingBackwards;
			CanMoveDown = !IsCharacterCollidingLeft;
			CanMoveUp = !IsCharacterCollidingRight;
		}
	}

	if (DeltaLocation != FVector::ZeroVector)
	{
		if (CurrentActorZRotation >= 0.0f and CurrentActorZRotation < 80.0f)
		{
			PushVelocity = FVector(PushSpeed, 0.f, 0.f);
			StopMovement();
		}
		else if (CurrentActorZRotation >= 80.0f and CurrentActorZRotation < 170.0f)
		{
			PushVelocity = FVector(0.f, PushSpeed, 0.f);
			StopMovement();
		}
		else if (CurrentActorZRotation >= 170.0f and CurrentActorZRotation < 260.0f)
		{
			PushVelocity = FVector(-PushSpeed, 0.f, 0.f);
			StopMovement();
		}
		else if (CurrentActorZRotation >= -90.0f and CurrentActorZRotation < 0.0f)
		{
			PushVelocity = FVector(0.f, -PushSpeed, 0.f);
			StopMovement();
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
