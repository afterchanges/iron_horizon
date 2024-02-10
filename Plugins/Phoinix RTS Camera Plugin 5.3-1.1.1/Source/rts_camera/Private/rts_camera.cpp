// Copyright Epic Games, Inc. All Rights Reserved.

#include "rts_camera.h"

#define LOCTEXT_NAMESPACE "Frts_cameraModule"

void Frts_cameraModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void Frts_cameraModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Frts_cameraModule, rts_camera)