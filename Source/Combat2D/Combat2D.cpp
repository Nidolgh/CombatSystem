// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat2D.h"

#define LOCTEXT_NAMESPACE "FCombat2DModule"

void FCombat2DModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCombat2DModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCombat2DModule, Combat2D)