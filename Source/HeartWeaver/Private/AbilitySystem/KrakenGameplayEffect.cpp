// Copyright Kraken's Den Studios. All rights reserved.


#include "AbilitySystem/KrakenGameplayEffect.h"

#include "GameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "GameplayEffectComponents/AssetTagsGameplayEffectComponent.h"

void UKrakenGameplayEffect::SetAssetTagInAssetTagsComponent(const FGameplayTag InTag)
{
	FInheritedTagContainer TagContainer;
	TagContainer.AddTag(InTag);
	
	TObjectPtr<UGameplayEffectComponent> Component = GEComponents[0];
	if (Component && Component->IsA<UAssetTagsGameplayEffectComponent>())
	{
		UAssetTagsGameplayEffectComponent* CastedComponent = Cast<UAssetTagsGameplayEffectComponent>(Component);
		CastedComponent->SetAndApplyAssetTagChanges(TagContainer);
	}
}
