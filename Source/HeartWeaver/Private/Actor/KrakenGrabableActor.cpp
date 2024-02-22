// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenGrabableActor.h"

#include "Characters/KrakenCharacter.h"
#include "Characters/KrakenCharacterMovementComponent.h"

AKrakenGrabableActor::AKrakenGrabableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}


void AKrakenGrabableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKrakenGrabableActor::Grab(AKrakenCharacter* TargetCharacter)
{
	if (TargetCharacter->GetGrabableActor() != nullptr)
	{
		return;
	}

	OnGrab();
	
	if (GrabableType == EGrabableType::Pickup)
	{
		this->AttachToActor(TargetCharacter, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("PickupSocket"));
		TargetCharacter->SetGrabableActor(this);
	}
	else if (GrabableType == EGrabableType::Movable)
	{
		TargetCharacter->GetKrakenCharacterMovementComponent()->PlayMontage(IdleToGrabMontage);
		this->AttachToActor(TargetCharacter, FAttachmentTransformRules::KeepWorldTransform, FName("GrabSocket"));
		TargetCharacter->SetGrabableActor(this);
		TargetCharacter->GetKrakenCharacterMovementComponent()->bOrientRotationToMovement = false;
	}
	this->Execute_Activate(this);
}

void AKrakenGrabableActor::Release(AKrakenCharacter* TargetCharacter)
{
	if (TargetCharacter->GetGrabableActor() != this)
	{
		return;
	}

	if (GrabableType == EGrabableType::Movable)
	{
		TargetCharacter->GetKrakenCharacterMovementComponent()->PlayMontage(GrabToIdleMontage);
		TargetCharacter->GetKrakenCharacterMovementComponent()->bOrientRotationToMovement = true;
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	else if (GrabableType == EGrabableType::Pickup)
	{
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	TargetCharacter->SetGrabableActor(nullptr);
	this->Execute_Deactivate(this);

	OnRelease();
}

void AKrakenGrabableActor::OnGrab_Implementation()
{
}

void AKrakenGrabableActor::OnRelease_Implementation()
{
}

FTransform AKrakenGrabableActor::GetTargetCharacterPosition_Implementation()
{
	return FTransform();
}


