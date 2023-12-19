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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(KrakenAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(KrakenAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	UKrakenAbilitySystemComponent* KrakenAbilitySystemComponent = Cast<UKrakenAbilitySystemComponent>(AbilitySystemComponent);
	KrakenAbilitySystemComponent->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
			for (const FGameplayTag& Tag: AssetTags)
			{
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
	);
}
