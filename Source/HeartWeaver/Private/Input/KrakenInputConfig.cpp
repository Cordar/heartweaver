// Copyright Kraken's Den Studios. All rights reserved.


#include "Input/KrakenInputConfig.h"

UKrakenInputConfig::UKrakenInputConfig()
{
}

const UInputAction* UKrakenInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return FindInputActionForTag(NativeInputActions, InputTag, bLogNotFound);
}

const UInputAction* UKrakenInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return FindInputActionForTag(AbilityInputActions, InputTag, bLogNotFound);
}

const UInputAction* UKrakenInputConfig::FindInputActionForTag(const TArray<FKrakenInputAction>& InputActions, const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FKrakenInputAction& Action : InputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find InputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
