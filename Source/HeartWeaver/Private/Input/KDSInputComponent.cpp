// Copyright Kraken's Den Studios. All rights reserved.


#include "Input/KDSInputComponent.h"

#include "Settings/KDSSettingsLocal.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"


UKDSInputComponent::UKDSInputComponent()
{
}

void UKDSInputComponent::AddInputMappings(const UKDSInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UKDSInputComponent::RemoveInputMappings(const UKDSInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UKDSInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
