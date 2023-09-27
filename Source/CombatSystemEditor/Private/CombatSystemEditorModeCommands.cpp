// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatSystemEditorModeCommands.h"
#include "CombatSystemEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "CombatSystemEditorModeCommands"

FCombatSystemEditorModeCommands::FCombatSystemEditorModeCommands()
	: TCommands<FCombatSystemEditorModeCommands>("CombatSystemEditorMode",
		NSLOCTEXT("CombatSystemEditorMode", "CombatSystemEditorModeCommands", "CombatSystem Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FCombatSystemEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);

	UI_COMMAND(OpenPluginWindow, "CombatSystemEditor", "Bring up CombatSystemEditor window", EUserInterfaceActionType::Button, FInputChord());
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FCombatSystemEditorModeCommands::GetCommands()
{
	return FCombatSystemEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
