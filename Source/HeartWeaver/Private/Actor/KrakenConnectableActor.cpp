// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenConnectableActor.h"

AKrakenConnectableActor::AKrakenConnectableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AKrakenConnectableActor::BeginPlay()
{
	Super::BeginPlay();
}

