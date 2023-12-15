// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DebugHelper.h"

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

	/*TraceClimbableSurfaces();
	TraceFromEyeHeight(100.f);*/
}

TArray<FHitResult> UKrakenCharacterMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape,bool bDrawPersistentShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false
	);

	return OutCapsuleTraceHitResults;
}

FHitResult UKrakenCharacterMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape,bool bDrawPersistentShapes) const
{
	//Eye Height Trace
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistentShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::LineTraceSingleForObjects(
	this,
	Start,
	End,
	ClimbableSurfaceTraceTypes,
	false,
	TArray<AActor*>(),
	DebugTraceType,
	OutHit,
	false
	);

	return OutHit;
}

void UKrakenCharacterMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if(bEnableClimb)
	{
		if(CanStartClimbing())
		{
			//Enter the climb state
			Debug::Print(TEXT("CAN start Climbing."));
		}
		else
		{
			Debug::Print(TEXT("CANNOT start Climbing."));

		}
	}
	else
	{
		//Stop Climbing
	}
}

bool UKrakenCharacterMovementComponent::CanStartClimbing()
{
	if(IsFalling()) return false;
	if(!TraceClimbableSurfaces()) return false;
	if(!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;
}

bool UKrakenCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::Move_Climb;
}

//Trace for climbable surfaces, return true if there are indeed valid surfaces, false otherwise
bool UKrakenCharacterMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	
	ClimbableSurfacesTraceResults = DoCapsuleTraceMultiByObject(Start, End, true, true);
	return !ClimbableSurfacesTraceResults.IsEmpty();
}

FHitResult UKrakenCharacterMovementComponent::TraceFromEyeHeight(const float TraceDistance, const float TraceStartOffset) const
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleByObject(Start,End,true, true);
}


