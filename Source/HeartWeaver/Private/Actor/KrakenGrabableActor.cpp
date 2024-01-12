// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenGrabableActor.h"

#include "Characters/KrakenCharacter.h"
#include "Components/SphereComponent.h"

AKrakenGrabableActor::AKrakenGrabableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
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
	
	if (GrabableType == EGrabableType::Pickup)
	{
		this->AttachToActor(TargetCharacter, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("PickupSocket"));
	}
	else if (GrabableType == EGrabableType::Movable)
	{
		this->AttachToActor(TargetCharacter, FAttachmentTransformRules::KeepWorldTransform, FName("MoveSocket"));
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
}


