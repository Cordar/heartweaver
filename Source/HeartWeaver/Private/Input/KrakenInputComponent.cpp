// Copyright Kraken's Den Studios. All rights reserved.


#include "Input/KrakenInputComponent.h"


UKrakenInputComponent::UKrakenInputComponent()
{
}

void UKrakenInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (const uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
