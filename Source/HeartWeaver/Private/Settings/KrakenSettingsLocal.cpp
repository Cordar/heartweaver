// Copyright Kraken's Den Studios. All rights reserved.


#include "Settings/KrakenSettingsLocal.h"

#include "GameFramework/GameUserSettings.h"

UKrakenSettingsLocal::UKrakenSettingsLocal()
{
}

UKrakenSettingsLocal* UKrakenSettingsLocal::Get()
{
    return nullptr;
    //return GEngine ? CastChecked<UKrakenSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}
