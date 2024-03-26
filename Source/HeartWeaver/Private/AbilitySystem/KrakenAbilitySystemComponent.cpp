// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/KrakenAbilitySystemComponent.h"

#include "AbilitySystem/KrakenGameplayTags.h"
#include "AbilitySystem/Abilities/KrakenGameplayAbility.h"

void UKrakenAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UKrakenAbilitySystemComponent::EffectApplied);
}

void UKrakenAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UKrakenGameplayAbility* KrakenAbility = Cast<UKrakenGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(KrakenAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UKrakenAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	const FKrakenGameplayTags& GameplayTags = FKrakenGameplayTags::Get();
	if (HasMatchingGameplayTag(GameplayTags.Tag_Gameplay_AbilityInputBlocked))
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UKrakenAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
}

void UKrakenAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	
	const FKrakenGameplayTags& GameplayTags = FKrakenGameplayTags::Get();
	if (HasMatchingGameplayTag(GameplayTags.Tag_Gameplay_AbilityInputBlocked))
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UKrakenAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec,
                                                  FActiveGameplayEffectHandle ActiveEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
