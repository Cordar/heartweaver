// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/Abilities/KrakenGameplayAbility.h"

#include "Characters/KrakenCharacter.h"

AKrakenCharacter* UKrakenGameplayAbility::GetKrakenCharacterFromActorInfo() const
{
	return CastChecked<AKrakenCharacter>(GetActorInfo().AvatarActor.Get());
}
