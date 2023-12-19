// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenCharacterBase.h"

#include "AbilitySystemComponent.h"

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
}

void AKrakenCharacterBase::InitAbilityActorInfo()
{
}

void AKrakenCharacterBase::InitializePrimaryAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()))
	check(DefaultPrimaryAttributes)
	
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

