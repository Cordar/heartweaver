// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

class ANA_Settings
{
	ANA_Settings() = delete;

public:
	static void RegisterSettings();
	static void UnregisterSettings();

private:
	static void resetConfigToDefaultValues(UObject* SettingsObject);
};
