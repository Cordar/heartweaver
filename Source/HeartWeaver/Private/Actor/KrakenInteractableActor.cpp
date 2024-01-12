// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenInteractableActor.h"

#include "Components/SphereComponent.h"

AKrakenInteractableActor::AKrakenInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

void AKrakenInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

