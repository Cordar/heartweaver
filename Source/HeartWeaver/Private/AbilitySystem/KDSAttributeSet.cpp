// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/KDSAttributeSet.h"

#include "Net/UnrealNetwork.h"

UKDSAttributeSet::UKDSAttributeSet()
{
	InitHealth(1.f);
	InitMaxHealth(1.f);
}

void UKDSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKDSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKDSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UKDSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKDSAttributeSet, Health, OldHealth);
}

void UKDSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKDSAttributeSet, MaxHealth, OldMaxHealth);
}
