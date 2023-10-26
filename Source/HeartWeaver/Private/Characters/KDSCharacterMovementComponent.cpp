// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KDSCharacterMovementComponent.h"

UKDSCharacterMovementComponent::UKDSCharacterMovementComponent()
{
}

bool UKDSCharacterMovementComponent::CanAttemptJump() const
{
	// Same as UCharacterMovementComponent's implementation but without the crouch check
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());
}

FRotator UKDSCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	// TODO: Implement for ability system tag movement stopped to be 0 (check Lyra Project)
	return Super::GetDeltaRotation(DeltaTime);
}

float UKDSCharacterMovementComponent::GetMaxSpeed() const
{
	// TODO: Implement for ability system tag movement stopped to be 0 (check Lyra Project)
	return Super::GetMaxSpeed();
}
