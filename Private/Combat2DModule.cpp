// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#include "Combat2DModule.h"

#include "C2DFrameInstructionsEditor/C2DFrameInstructionsEditor.h"
#include "MovesetEditor/MovesetEditor.h"

#include "Modules/ModuleManager.h"
#include "Combat2DStyle.h"
#include "AssetToolsModule.h"

const FName C2DFrameInstructionsEditorAppIdentifier = FName(TEXT("C2DFrameInstructionsEditorApp"));
const FName MovesetEditorAppIdentifier = FName(TEXT("MovesetEditorApp"));

#define LOCTEXT_NAMESPACE "FCombat2DEditorModule"

IMPLEMENT_GAME_MODULE(FCombat2DModule, Combat2DEditor);

//DEFINE_LOG_CATEGORY(LogCombat2DModule);

/** Constructor */
FCombat2DModule::FCombat2DModule() { }

/**
* Called right after the module DLL has been loaded and the module object has been created
*/
void FCombat2DModule::StartupModule()
{
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_C2DFrameInstructions()));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_Moveset()));

	AssetGategoryType = AssetTools.RegisterAdvancedAssetCategory("Combat2DAssets", FText::FromString("Combat2DAssets"));

	FCombat2DStyle::Initialize();
}

void FCombat2DModule::ShutdownModule()
{
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		// Unregister our custom created assets from the AssetTools
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 i = 0; i < CreatedAssetTypeActions.Num(); ++i)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[i].ToSharedRef());
		}
	}

	FCombat2DStyle::Shutdown();

	CreatedAssetTypeActions.Empty();
}

TSharedRef<IC2DFrameInstructionsEditor> FCombat2DModule::CreateC2DFrameInstructionsEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UC2DFrameInstructions* C2DFrameInstructions)
{
	TSharedRef<FC2DFrameInstructionsEditor> NewC2DFrameInstructionsEditor(new FC2DFrameInstructionsEditor());
	NewC2DFrameInstructionsEditor->InitC2DFrameInstructionsEditor(Mode, InitToolkitHost, C2DFrameInstructions);
	return NewC2DFrameInstructionsEditor;
}

TSharedRef<IMovesetEditor> FCombat2DModule::CreateMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMoveset* Moveset)
{
	TSharedRef<FMovesetEditor> NewMovesetEditor(new FMovesetEditor());
	NewMovesetEditor->InitMovesetEditor(Mode, InitToolkitHost, Moveset);
	return NewMovesetEditor;
}

void FCombat2DModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

TSharedPtr<FExtensibilityManager> FCombat2DModule::GetMenuExtensibilityManager()
{
	return MenuExtensibilityManager;
}

TSharedPtr<FExtensibilityManager> FCombat2DModule::GetToolBarExtensibilityManager()
{
	return ToolBarExtensibilityManager;
}

EAssetTypeCategories::Type FCombat2DModule::GetAssetCategoryType() const
{
	return AssetGategoryType;
}

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////

FVector PaperAxisX(1.0f, 0.0f, 0.0f);
FVector PaperAxisY(0.0f, 0.0f, 1.0f);
FVector PaperAxisZ(0.0f, 1.0f, 0.0f);