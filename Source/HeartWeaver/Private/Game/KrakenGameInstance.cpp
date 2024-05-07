// Copyright Kraken's Den Studios. All rights reserved.


#include "Game/KrakenGameInstance.h"

#include "SpudSubsystem.h"

void UKrakenGameInstance::Init()
{
	Super::Init();

	GetSpudSubsystem(GetWorld())->AddPersistentGlobalObjectWithName(this, "KrakenGameInstance");
}
