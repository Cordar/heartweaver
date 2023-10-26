// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KDSCharacterBase.h"

AKDSCharacterBase::AKDSCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

}

void AKDSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

