// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KDSLocalPlayer.h"

#include "Settings/KDSSettingsLocal.h"

UKDSLocalPlayer::UKDSLocalPlayer()
{
}

UKDSSettingsLocal* UKDSLocalPlayer::GetLocalSettings() const
{
	return UKDSSettingsLocal::Get();
}
