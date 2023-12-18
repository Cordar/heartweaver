// Copyright Kraken's Den Studios. All rights reserved.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "AbilitySystem/KrakenAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UKrakenAttributeSet* KrakenAttributeSet = Cast<UKrakenAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(KrakenAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(KrakenAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UKrakenAttributeSet* KrakenAttributeSet = Cast<UKrakenAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		KrakenAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		KrakenAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	UKrakenAbilitySystemComponent* KrakenAbilitySystemComponent = Cast<UKrakenAbilitySystemComponent>(AbilitySystemComponent);
	KrakenAbilitySystemComponent->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag: AssetTags)
			{
				const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);
			}
		}
	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}
