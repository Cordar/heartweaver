// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/Abilities/KrakenGameplayAbility_Jump.h"

#include "Characters/KrakenCharacter.h"

UKrakenGameplayAbility_Jump::UKrakenGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UKrakenGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                     const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AKrakenCharacterBase* KrakenCharacter = Cast<AKrakenCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!KrakenCharacter || !KrakenCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UKrakenGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CharacterJumpStop();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKrakenGameplayAbility_Jump::CharacterJumpStart()
{
	if (AKrakenCharacterBase* KrakenCharacterBase = GetKrakenCharacterFromActorInfo())
	{
		if (KrakenCharacterBase->IsLocallyControlled() && !KrakenCharacterBase->bPressedJump)
		{
			KrakenCharacterBase->UnCrouch();
			KrakenCharacterBase->Jump();
		}
	}
}

void UKrakenGameplayAbility_Jump::CharacterJumpStop()
{
	if (AKrakenCharacterBase* KrakenCharacterBase = GetKrakenCharacterFromActorInfo())
	{
		if (KrakenCharacterBase->IsLocallyControlled() && KrakenCharacterBase->bPressedJump)
		{
			KrakenCharacterBase->StopJumping();
		}
	}
}
