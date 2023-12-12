// Copyright Kraken's Den Studios. All rights reserved.


#include "Input/KrakenInputComponent.h"

#include "EnhancedInputSubsystems.h"


UKrakenInputComponent::UKrakenInputComponent()
{
}

void UKrakenInputComponent::AddInputMappings(const UKrakenInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UKrakenInputComponent::RemoveInputMappings(const UKrakenInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UKrakenInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
