// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterBase.h"
#include "DebugHelper.h"

AKrakenCharacterBase::AKrakenCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

UAbilitySystemComponent* AKrakenCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AKrakenCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	Debug::Print(TEXT("Debug Working")); 
}


