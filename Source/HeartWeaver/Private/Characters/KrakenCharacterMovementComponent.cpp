// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterMovementComponent.h"

#include "Kismet/KismetSystemLibrary.h"

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

void UKrakenCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceClimbableSurfaces();
}

TArray<FHitResult> UKrakenCharacterMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;
	
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		bShowDebugShape? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		OutCapsuleTraceHitResults,
		false
	);

	return OutCapsuleTraceHitResults;
}

void UKrakenCharacterMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	
	DoCapsuleTraceMultiByObject(Start, End, true);
}
