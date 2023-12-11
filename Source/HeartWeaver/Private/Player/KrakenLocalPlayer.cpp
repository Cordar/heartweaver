// Copyright Kraken's Den Studios. All rights reserved.


#include "Player/KrakenLocalPlayer.h"

#include "Settings/KrakenSettingsLocal.h"

UKrakenLocalPlayer::UKrakenLocalPlayer()
{
}

UKrakenSettingsLocal* UKrakenLocalPlayer::GetLocalSettings() const
{
	return UKrakenSettingsLocal::Get();
}
