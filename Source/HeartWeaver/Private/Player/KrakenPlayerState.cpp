// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KrakenPlayerState.h"

#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "AbilitySystem/KrakenAttributeSet.h"

AKrakenPlayerState::AKrakenPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UKrakenAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UKrakenAttributeSet>(TEXT("AttributeSet"));
	
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AKrakenPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
