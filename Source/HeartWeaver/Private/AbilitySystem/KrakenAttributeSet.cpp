// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/KrakenAttributeSet.h"

#include "Net/UnrealNetwork.h"

UKrakenAttributeSet::UKrakenAttributeSet()
{
	InitHealth(1.f);
	InitMaxHealth(1.f);
}

void UKrakenAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UKrakenAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UKrakenAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UKrakenAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.f, GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("Health: %f"), NewValue)
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("MaxHealth: %f"), NewValue)
	}
}

void UKrakenAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKrakenAttributeSet, Health, OldHealth);
}

void UKrakenAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UKrakenAttributeSet, MaxHealth, OldMaxHealth);
}
