// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "ANA_Settings.h"
#include "ANA_EditorConfig.h"
#include "ANA_ProjectConfig.h"
#include "../Copy_ApplicationRestartRequiredNotification.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleManager.h"
#include "../Commands/ANA_ReleaseNotifier.h"

void ANA_Settings::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
#define LOCTEXT_NAMESPACE "FAutoNodeArrangerModule"
		ISettingsSectionPtr EditorSettingsSection = SettingsModule->RegisterSettings("Editor", "Plugins", "Auto Node Arranger",
			LOCTEXT("RuntimeGeneralSettingsName", "Auto Node Arranger"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Editor configuration for Auto Node Arranger module"),
			UANA_EditorConfig::Get()
		);
		ISettingsSectionPtr ProjectSettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Auto Node Arranger",
			LOCTEXT("RuntimeGeneralSettingsName", "Auto Node Arranger"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Project configuration for Auto Node Arranger module"),
			UANA_ProjectConfig::Get()
		);
#undef LOCTEXT_NAMESPACE
		static FCopy_ApplicationRestartRequiredNotification ApplicationRestartRequiredNotification;
		EditorSettingsSection->OnModified().BindLambda([]()
		{
			if (UANA_EditorConfig::CheckShowNewFeatures())
				ANA_ReleaseNotifier::ShowNotification(CompleteActionType::NONE);
			UANA_EditorConfig::Get()->SaveConfig();
			return true;
		});
		EditorSettingsSection->OnResetDefaults().BindLambda([EditorSettingsSection]()
		{
			resetConfigToDefaultValues(UANA_EditorConfig::Get());
			ApplicationRestartRequiredNotification.OnRestartRequired();
			return true;
		});
		ProjectSettingsSection->OnModified().BindLambda([]()
		{
			UANA_ProjectConfig::Get()->SaveConfig();
			return true;
		});
		ProjectSettingsSection->OnResetDefaults().BindLambda([EditorSettingsSection]()
		{
			resetConfigToDefaultValues(UANA_ProjectConfig::Get());
			ApplicationRestartRequiredNotification.OnRestartRequired();
			return true;
		});
	}
}

void ANA_Settings::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Auto Node Arranger");
		SettingsModule->UnregisterSettings("Project", "Plugins", "Auto Node Arranger");
	}
}

void ANA_Settings::resetConfigToDefaultValues(UObject* SettingsObject)
{
	FString ConfigName = SettingsObject->GetClass()->GetConfigName();

	GConfig->EmptySection(*SettingsObject->GetClass()->GetPathName(), ConfigName);
	GConfig->Flush(false);

	FConfigCacheIni::LoadGlobalIniFile(ConfigName, *FPaths::GetBaseFilename(ConfigName), nullptr, true);

	SettingsObject->ReloadConfig(nullptr, nullptr, UE::LCPF_PropagateToInstances | UE::LCPF_PropagateToChildDefaultObjects);
}