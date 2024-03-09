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
}

void AKrakenGrabableActor::Release(AKrakenCharacter* TargetCharacter)
{
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


