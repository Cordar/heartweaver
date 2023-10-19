// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"

#include "KDSSettingsLocal.generated.h"

/**
 * 
 */
UCLASS()
class HEARTWEAVER_API UKDSSettingsLocal : public UEnhancedInputUserSettings
{
	GENERATED_BODY()

public:
	UKDSSettingsLocal();

	static UKDSSettingsLocal* Get();	
};
