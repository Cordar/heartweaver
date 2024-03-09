// Copyright Kraken's Den Studios. All rights reserved.


#include "AnimInstance/KrakenCharacterAnimInstance.h"

#include "Characters/KrakenCharacter.h"
#include "Characters/KrakenCharacterMovementComponent.h"
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
	GetIsGoingBackwards();
	GetIsCrouching();
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

void UKrakenCharacterAnimInstance::GetIsGoingBackwards()
{
	const FVector Velocity = ControlledCharacter->GetVelocity().GetSafeNormal();
	bIsGoingBackwards = FVector::DotProduct(ControlledCharacter->GetActorForwardVector(), Velocity) < 0;
}

void UKrakenCharacterAnimInstance::GetIsCrouching()
{
	bIsCrouching = CharacterMovementComponent->IsCrouching();
}
