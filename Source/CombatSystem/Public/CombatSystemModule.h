#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMovesetAssetTypeActions;
/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FCombatSystemModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterConsoleCommands() const;
};
