#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMovesetAssetTypeActions;

// Editor mode constants
struct COMBATSYSTEMEDITOR_API FCombatSystemEditModes
{
	const static FEditorModeID EM_CombatSystemID;
};

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FCombatSystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	void ExtendToolbar() const;
	
private:
	void RegisterMenus();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<FMovesetAssetTypeActions> MovesetAssetTypeActionsAssetTypeActions;

};
