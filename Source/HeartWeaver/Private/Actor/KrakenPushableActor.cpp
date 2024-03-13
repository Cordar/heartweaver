// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenPushableActor.h"

// Sets default values
AKrakenPushableActor::AKrakenPushableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKrakenPushableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKrakenPushableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

