// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat2DEditorModule.h"
#include "EditorModeRegistry.h"

#include "Combat2DStyle.h"

#include "CombatFlipbookEditor/CombatFlipbookEditor.h"
#include "CombatMovesetEditor/CombatMovesetEditor.h"

#include "CombatFlipbookAssetTypeActions.h"
#include "CombatMovesetAssetTypeActions.h"

#include "CombatFlipbookEditor/SpriteEditing/CombatSpriteGeometryEditMode.h"

#define LOCTEXT_NAMESPACE "FCombat2DEditorModule"

void FCombat2DEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FCombatFlipbookAssetTypeActions()));
	//RegisterAssetTypeAction(AssetTools, MakeShareable(new FCombatMovesetAssetTypeActions()));

	FEditorModeRegistry::Get().RegisterMode<FCombatSpriteGeometryEditMode>(
		FCombatSpriteGeometryEditMode::EM_CombatSpriteGeometry,
		LOCTEXT("CombatSpriteGeometryEditMode", "Combat Sprite Geometry Editor"),
		FSlateIcon(),
		false);

	FCombat2DStyle::Initialize();
}

void FCombat2DEditorModule::ShutdownModule()
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

	FEditorModeRegistry::Get().UnregisterMode(FCombatSpriteGeometryEditMode::EM_CombatSpriteGeometry);

	CreatedAssetTypeActions.Empty();
	
	FCombat2DStyle::Shutdown();
}

TSharedRef<FCombatFlipbookEditor> FCombat2DEditorModule::CreateCombatFlipbookEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UCombatFlipbook* CombatFlipbookAsset)
{
	TSharedRef<FCombatFlipbookEditor> NewEditor(new FCombatFlipbookEditor());
	NewEditor->InitCombatFlipbookEditor(Mode, InitToolkitHost, CombatFlipbookAsset);
	return NewEditor;
}

TSharedRef<FCombatMovesetEditor> FCombat2DEditorModule::CreateCombatMovesetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, UCombatMoveset* CombatMovesetAsset)
{
	TSharedRef<FCombatMovesetEditor> NewEditor(new FCombatMovesetEditor());
	NewEditor->InitCombatMovesetEditor(Mode, InitToolkitHost, CombatMovesetAsset);
	return NewEditor;
}

void FCombat2DEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE

const FName CombatFlipbookEditorAppIdentifier = FName(TEXT("CombatFlipbookEditorApp"));
const FName CombatMovesetEditorAppIdentifier = FName(TEXT("CombatMovesetEditorApp"));

IMPLEMENT_MODULE(FCombat2DEditorModule, Combat2DEditor)