// Copyright Kraken's Den Studios. All rights reserved.


#include "AnimInstance/KrakenCharacterAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Characters/KrakenCharacter.h"
#include "Characters/KrakenCharacterMovementComponent.h"
#include "Characters/KrakenPushComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UKrakenCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ControlledCharacter = Cast<AKrakenCharacter>(TryGetPawnOwner());

	if (ControlledCharacter)
	{
		CharacterMovementComponent = ControlledCharacter->GetKrakenCharacterMovementComponent();
	}
}

void UKrakenCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ControlledCharacter || !CharacterMovementComponent) { return; }

	GetGroundSpeed();
	GetAirSpeed();
	GetShouldMove();
	GetIsFalling();
	GetIsClimbing();
	GetClimbVelocity();
	GetIsLayingOnFloor();
	GetIsCrouching();
	GetIsHoldingAnObject();
	GetIsMovingAnObject();
	GetPushDirection();
	GetPushSpeed();
}

void UKrakenCharacterAnimInstance::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(ControlledCharacter->GetVelocity());
}

void UKrakenCharacterAnimInstance::GetAirSpeed()
{
	AirSpeed = ControlledCharacter->GetVelocity().Z;
}

void UKrakenCharacterAnimInstance::GetShouldMove()
{
	bShouldMove = CharacterMovementComponent->GetCurrentAcceleration().Size()>0&&GroundSpeed>5.f&&!bIsFalling;
}

void UKrakenCharacterAnimInstance::GetIsFalling()
{
	bIsFalling = CharacterMovementComponent->IsFalling();
}

void UKrakenCharacterAnimInstance::GetIsClimbing()
{
	bIsClimbing = CharacterMovementComponent->IsClimbing();
}

void UKrakenCharacterAnimInstance::GetClimbVelocity()
{
	ClimbVelocity = CharacterMovementComponent->GetUnrotatedClimbVelocity();
}

void UKrakenCharacterAnimInstance::GetIsLayingOnFloor()
{
	bIsLayingOnFloor = ControlledCharacter->IsLayingOnFloor();
}

void UKrakenCharacterAnimInstance::GetIsCrouching()
{
	bIsCrouching = CharacterMovementComponent->IsCrouching();
}

void UKrakenCharacterAnimInstance::GetIsHoldingAnObject()
{
	bIsHoldingAnObject = ControlledCharacter->GetPushComponent()->IsHoldingObject();
}

void UKrakenCharacterAnimInstance::GetIsMovingAnObject()
{
	bIsMovingAnObject = ControlledCharacter->GetPushComponent()->IsMovingAnObject;
}

void UKrakenCharacterAnimInstance::GetPushDirection()
{
	if (const UKrakenPushComponent* PushComponent = ControlledCharacter->GetPushComponent())
	{
		PushDirection = UKismetAnimationLibrary::CalculateDirection(PushComponent->PushVelocity, PushComponent->PushRotation);
	}
}

void UKrakenCharacterAnimInstance::GetPushSpeed()
{
	if (const UKrakenPushComponent* PushComponent = ControlledCharacter->GetPushComponent())
	{
		PushSpeed = PushComponent->PushVelocity != FVector::ZeroVector ? PushComponent->PushSpeed : 0.f;
	}
}
