// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterMovementComponent.h"

UKrakenCharacterMovementComponent::UKrakenCharacterMovementComponent()
{
}

bool UKrakenCharacterMovementComponent::CanAttemptJump() const
{
	// Same as UCharacterMovementComponent's implementation but without the crouch check
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());
}

FRotator UKrakenCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	return Super::GetDeltaRotation(DeltaTime);
}

float UKrakenCharacterMovementComponent::GetMaxSpeed() const
{
	// TODO: Implement for ability system tag movement stopped to be 0 (check Lyra Project)
	return Super::GetMaxSpeed();
}
