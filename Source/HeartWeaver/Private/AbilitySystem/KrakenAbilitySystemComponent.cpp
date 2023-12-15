// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/KrakenAbilitySystemComponent.h"

void UKrakenAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UKrakenAbilitySystemComponent::EffectApplied);
}

void UKrakenAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec,
                                                  FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	for (const FGameplayTag& Tag: TagContainer)
	{
		
	}
}
