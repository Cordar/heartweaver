// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/Abilities/KrakenGameplayAbility.h"

#include "Characters/KrakenCharacter.h"

AKrakenCharacter* UKrakenGameplayAbility::GetKrakenCharacterFromActorInfo() const
{
	AActor* AvatarActor = GetActorInfo().AvatarActor.Get();

	if (AKrakenCharacter* KrakenCharacter = Cast<AKrakenCharacter>(AvatarActor)) return KrakenCharacter;
	return nullptr;
}
