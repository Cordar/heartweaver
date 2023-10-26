// Copyright Kraken's Den Studios. All rights reserved.


#include "Input/KDSInputConfig.h"

UKDSInputConfig::UKDSInputConfig()
{
}

const UInputAction* UKDSInputConfig::FindInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FKDSInputAction& Action : InputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
