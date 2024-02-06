// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenGrabableActor.h"

#include "Characters/KrakenCharacter.h"

AKrakenGrabableActor::AKrakenGrabableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKrakenGrabableActor::OnRelease_Implementation()
{
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
	}
	else if (GrabableType == EGrabableType::Movable)
	{
		this->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
	TargetCharacter->SetGrabableActor(this);
}

void AKrakenGrabableActor::Release(AKrakenCharacter* TargetCharacter)
{
	if (TargetCharacter->GetGrabableActor() != this)
	{
		return;
	}
	
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	TargetCharacter->SetGrabableActor(nullptr);

	OnRelease();
}

void AKrakenGrabableActor::OnGrab_Implementation()
{
}


