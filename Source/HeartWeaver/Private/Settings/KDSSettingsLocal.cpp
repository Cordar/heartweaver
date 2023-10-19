// Copyright Kraken's Den Studios. All rights reserved.


#include "Settings/KDSSettingsLocal.h"

#include "GameFramework/GameUserSettings.h"

UKDSSettingsLocal::UKDSSettingsLocal()
{
}

UKDSSettingsLocal* UKDSSettingsLocal::Get()
{
    return GEngine ? CastChecked<UKDSSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}
