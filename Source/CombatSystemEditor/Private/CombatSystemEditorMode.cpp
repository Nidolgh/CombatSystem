// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatSystemEditorMode.h"
#include "CombatSystemEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "CombatSystemEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/CombatSystemSimpleTool.h"
#include "Tools/CombatSystemInteractiveTool.h"

// step 2: register a ToolBuilder in FCombatSystemEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "CombatSystemEditorMode"

const FEditorModeID UCombatSystemEditorMode::EM_CombatSystemEditorModeId = TEXT("EM_CombatSystemEditorMode");

FString UCombatSystemEditorMode::SimpleToolName = TEXT("CombatSystem_ActorInfoTool");
FString UCombatSystemEditorMode::InteractiveToolName = TEXT("CombatSystem_MeasureDistanceTool");


UCombatSystemEditorMode::UCombatSystemEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UCombatSystemEditorMode::EM_CombatSystemEditorModeId,
		LOCTEXT("ModeName", "CombatSystem"),
		FSlateIcon(),
		true);
}


UCombatSystemEditorMode::~UCombatSystemEditorMode()
{
}


void UCombatSystemEditorMode::ActorSelectionChangeNotify()
{
}

void UCombatSystemEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FCombatSystemEditorModeCommands& SampleToolCommands = FCombatSystemEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UCombatSystemSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UCombatSystemInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UCombatSystemEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FCombatSystemEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UCombatSystemEditorMode::GetModeCommands() const
{
	return FCombatSystemEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
