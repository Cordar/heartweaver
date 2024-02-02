// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetMathLibrary.h"

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
	if(IsClimbing())
	{
		return MaxClimbSpeed;
	}
	return Super::GetMaxSpeed();
}

float UKrakenCharacterMovementComponent::GetMaxAcceleration() const
{
	if(IsClimbing())
	{
		return MaxClimbAcceleration;
	}
	return Super::GetMaxAcceleration();
}

FVector UKrakenCharacterMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
	const bool bIsPlayingRMMontage = IsFalling() && OwningCharacterAnimInstance && OwningCharacterAnimInstance->IsAnyMontagePlaying();
	if (bIsPlayingRMMontage)
	{
		return RootMotionVelocity;
	}
	
	return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
}

void UKrakenCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	if(OwningCharacterAnimInstance)
	{
		OwningCharacterAnimInstance->OnMontageEnded.AddDynamic(this, &UKrakenCharacterMovementComponent::OnClimbMontageEnded);
		OwningCharacterAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UKrakenCharacterMovementComponent::OnClimbMontageEnded);
	}
}

void UKrakenCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UKrakenCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	if(IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(50.f);
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::Move_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);

		const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
		const FRotator CleanStandRotation = FRotator(0.f, DirtyRotation.Yaw,0.f);
		UpdatedComponent->SetRelativeRotation(CleanStandRotation);
		
		StopMovementImmediately();
	}
	
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UKrakenCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if(IsClimbing())
	{
		PhysClimb(DeltaTime, Iterations);
	}
	Super::PhysCustom(DeltaTime, Iterations);
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
	UKismetSystemLibrary::CapsuleTraceMulti(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
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
	UKismetSystemLibrary::LineTraceSingle(
	this,
	Start,
	End,
	UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),
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
			if (IsFalling())
			{
				StartClimbing();
			}
			else
			{
				check(IdleToClimbMontage);
				PlayClimbMontage(IdleToClimbMontage);
			}
		}
	}
	else
	{
		StopClimbing();
	}
}

bool UKrakenCharacterMovementComponent::CanStartClimbing()
{
	if(!TraceClimbableSurfaces()) return false;
	if(!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;
}

void UKrakenCharacterMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom,ECustomMovementMode::Move_Climb);
}

void UKrakenCharacterMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UKrakenCharacterMovementComponent::PhysClimb(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	TraceClimbableSurfaces();
	ProcessClimbableSurfaceInfo();
	
	if (CheckShouldStopClimbing() || CheckHasReachedFloor())
	{
		StopClimbing();
	}
	
	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		CalcVelocity(DeltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(DeltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * DeltaTime;
	FHitResult Hit(1.f);

	// Handle climb rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(DeltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, DeltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}

	SnapMovementToClimbableSurfaces(DeltaTime);

	if(CheckHasReachedLedge())
	{
		PlayClimbMontage(ClimbToTopMontage);
	}
}

void UKrakenCharacterMovementComponent::ProcessClimbableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if(ClimbableSurfacesTraceResults.IsEmpty()) return;

	for(const FHitResult& TracedHitResult:ClimbableSurfacesTraceResults)
	{
		CurrentClimbableSurfaceLocation += TracedHitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TracedHitResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbableSurfacesTraceResults.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();

}

bool UKrakenCharacterMovementComponent::CheckShouldStopClimbing()
{
	if (ClimbableSurfacesTraceResults.IsEmpty()) return true;

	const float DotResultUp = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
	const float DotResultLeft = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::LeftVector);
	const float DotResultRight = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::RightVector);
	
	const float DegreeDiffUp = FMath::RadiansToDegrees(FMath::Acos(DotResultUp));
	const float DegreeDiffLeft = FMath::RadiansToDegrees(FMath::Acos(DotResultLeft));
	const float DegreeDiffRight = FMath::RadiansToDegrees(FMath::Acos(DotResultRight));

	if(DegreeDiffUp <= StopClimbUpAngle)
	{
		return true;
	}

	return false;
}

bool UKrakenCharacterMovementComponent::CheckHasReachedFloor()
{
	const FVector DownVector = UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits = DoCapsuleTraceMultiByObject(Start, End, false);

	if(PossibleFloorHits.IsEmpty()) return false;

	for (const FHitResult& HitResult : PossibleFloorHits)
	{
		const bool bFloorReached =
		FVector::Parallel(-HitResult.ImpactNormal, FVector::UpVector) &&
			GetUnrotatedClimbVelocity().Z < -10.f;

		if (bFloorReached)
		{
			return true;
		}
	}
	return false;
}

FQuat UKrakenCharacterMovementComponent::GetClimbRotation(float DeltaTime)
{
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

	if(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}

	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.f);
}

void UKrakenCharacterMovementComponent::SnapMovementToClimbableSurfaces(float DeltaTime)
{
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	const FVector ProjectedCharacterToSurface =
		(CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

	UpdatedComponent->MoveComponent(
		SnapVector*DeltaTime*MaxClimbSpeed,
		UpdatedComponent->GetComponentQuat(),
		true);
}

bool UKrakenCharacterMovementComponent::CheckHasReachedLedge()
{
	FHitResult LedgeHitResult = TraceFromEyeHeight(100.f, CheckLedgeTraceOffset);

	if (!LedgeHitResult.bBlockingHit)
	{
		const FVector WalkableSurfaceTraceStart = LedgeHitResult.TraceEnd;
		const FVector DownVector = -UpdatedComponent->GetUpVector();
		const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;

		FHitResult WalkableSurfaceHitResult = DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd);

		if(WalkableSurfaceHitResult.bBlockingHit && GetUnrotatedClimbVelocity().Z > 10.f)
		{
			return true;
		}
	}

	return false;
}

void UKrakenCharacterMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if(!MontageToPlay) return;
	if(!OwningCharacterAnimInstance) return;
	if(OwningCharacterAnimInstance->IsAnyMontagePlaying()) return;

	OwningCharacterAnimInstance->Montage_Play(MontageToPlay);
}

void UKrakenCharacterMovementComponent::OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == IdleToClimbMontage)
	{
		StartClimbing();
	}
	else
	{
		SetMovementMode(MOVE_Walking);
	}
}

bool UKrakenCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::Move_Climb;
}

FVector UKrakenCharacterMovementComponent::GetUnrotatedClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

//Trace for climbable surfaces, return true if there are indeed valid surfaces, false otherwise
bool UKrakenCharacterMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	
	ClimbableSurfacesTraceResults = DoCapsuleTraceMultiByObject(Start, End);
	return !ClimbableSurfacesTraceResults.IsEmpty();
}

FHitResult UKrakenCharacterMovementComponent::TraceFromEyeHeight(const float TraceDistance, const float TraceStartOffset) const
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleByObject(Start,End);
}


