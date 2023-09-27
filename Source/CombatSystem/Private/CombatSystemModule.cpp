#include "CombatSystemModule.h"

#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "CombatSystemModule"

void FCombatSystemModule::StartupModule()
{
	RegisterConsoleCommands();
}

void FCombatSystemModule::ShutdownModule()
{
}

void FCombatSystemModule::RegisterConsoleCommands() const
{
	IConsoleManager::Get().RegisterConsoleVariable(TEXT("CombatSystem.DrawDebugShapes"),
	   0,
	   TEXT("Draws debug shapes for set anim notify collisions shapes in-game.\n")
		TEXT("<=0: off\n")
		TEXT("  1: Draws Debug shapes in game view"),
	   ECVF_Scalability | ECVF_RenderThreadSafe);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCombatSystemModule, CombatSystemEditorMode)