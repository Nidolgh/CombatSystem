// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "IAssetTools.h"

#include "Combat2DEditor/Classes/AssetTypeActions_C2DFrameInstructions.h"
#include "Combat2DEditor/Classes/AssetTypeActions_Moveset.h"

class IC2DFrameInstructionsEditor;
class UC2DFrameInstructions;

class IMovesetEditor;
class UMoveset;

extern const FName C2DFrameInstructionsEditorAppIdentifier;
extern const FName MovesetEditorAppIdentifier;

/**
 * Custom Asset editor module interface
 */
class ICombat2DModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
	/**
	 * Creates a new custom asset editor.
	 */
	virtual TSharedRef<IC2DFrameInstructionsEditor> CreateC2DFrameInstructionsEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UC2DFrameInstructions* C2DFrameInstructions) = 0;
	virtual TSharedRef<IMovesetEditor> CreateMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMoveset* Moveset) = 0;

	virtual EAssetTypeCategories::Type GetAssetCategoryType() const = 0;
};

/**
* StaticMesh editor module
*/
class FCombat2DModule : public ICombat2DModule
{
public:
	/** Constructor */
	FCombat2DModule();

	/**
	* Called right after the module DLL has been loaded and the module object has been created
	*/
	virtual void StartupModule() override;

	/**
	* Called before the module is unloaded, right before the module object is destroyed.
	*/
	virtual void ShutdownModule() override;

	virtual TSharedRef<IC2DFrameInstructionsEditor> CreateC2DFrameInstructionsEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UC2DFrameInstructions* C2DFrameInstructions) override;

	virtual TSharedRef<IMovesetEditor> CreateMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMoveset* Moveset) override;

	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

	/** Gets the extensibility managers for outside entities to extend static mesh editor's menus and toolbars */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override;
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override;

	virtual EAssetTypeCategories::Type GetAssetCategoryType() const override;

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;

	EAssetTypeCategories::Type AssetGategoryType;
};