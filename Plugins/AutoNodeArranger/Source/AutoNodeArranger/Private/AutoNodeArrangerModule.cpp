// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "AutoNodeArrangerModule.h"
#include "Commands/ANA_Commands.h"
#include "Commands/ANA_InputProcessor.h"
#include "Commands/ANA_ReleaseNotifier.h"
#include "Config/ANA_Settings.h"

void FAutoNodeArrangerModule::StartupModule()
{
	ANA_Commands::Register();
	ANA_Commands::BindAllCommands();
	ANA_Settings::RegisterSettings();
	ANA_InputProcessor::RegisterProcessor();
	ANA_ReleaseNotifier::RegisterCheckVersion();
}

void FAutoNodeArrangerModule::ShutdownModule()
{
	ANA_InputProcessor::UnregisterProcessor();
	ANA_Settings::UnregisterSettings();
	ANA_Commands::UnbindAllCommands();
	ANA_Commands::Unregister();
}
	
IMPLEMENT_MODULE(FAutoNodeArrangerModule, AutoNodeArranger)