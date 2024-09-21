// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"

#include "KrakenSettingsLocal.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKrakenSettingsLocal : public UEnhancedInputUserSettings
{
	GENERATED_BODY()

public:
	UKrakenSettingsLocal();

	static UKrakenSettingsLocal* Get();
};
