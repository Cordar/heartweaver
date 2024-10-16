// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicSplineCamera.h"
#include "Modules/ModuleManager.h"


#define LOCTEXT_NAMESPACE "FDynamicSplineCameraModule"

void FDynamicSplineCameraModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FDynamicSplineCameraModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicSplineCameraModule, DynamicSplineCamera)


