// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatSystemEditorModeToolkit.h"
#include "CombatSystemEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "CombatSystemEditorModeToolkit"

FCombatSystemEditorModeToolkit::FCombatSystemEditorModeToolkit()
{
}

void FCombatSystemEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FCombatSystemEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FCombatSystemEditorModeToolkit::GetToolkitFName() const
{
	return FName("CombatSystemEditorMode");
}

FText FCombatSystemEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "CombatSystemEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
