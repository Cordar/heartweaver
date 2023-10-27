// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KDSPlayerState.h"

#include "AbilitySystem/KDSAbilitySystemComponent.h"
#include "AbilitySystem/KDSAttributeSet.h"

AKDSPlayerState::AKDSPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UKDSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode()

	AttributeSet = CreateDefaultSubobject<UKDSAttributeSet>(TEXT("AttributeSet"));
	
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AKDSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
