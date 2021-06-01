// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Combat2DStyle.h"

class FFlipbookDataEditorCommands : public TCommands<FFlipbookDataEditorCommands>
{
public:
	FFlipbookDataEditorCommands()
		: TCommands<FFlipbookDataEditorCommands>(
			TEXT("FlipbookDataEditor"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "FlipbookDataEditor", "FlipbookData Editor"), // Localized context name for displaying
			NAME_None, // Parent
			FCombat2DStyle::Get()->GetStyleSetName() // Icon Style Set
			)
	{
	}

	// TCommand<> interface
	virtual void RegisterCommands() override;
	// End of TCommand<> interface

public:
	// Show toggles
	TSharedPtr<FUICommandInfo> SetShowGrid;
	TSharedPtr<FUICommandInfo> SetShowSourceTexture;
	TSharedPtr<FUICommandInfo> SetShowBounds;
	TSharedPtr<FUICommandInfo> SetShowCollision;
	
	TSharedPtr<FUICommandInfo> SetShowSockets;

	TSharedPtr<FUICommandInfo> SetShowPivot;
	TSharedPtr<FUICommandInfo> SetShowMeshEdges;

	// Source region edit mode
	TSharedPtr<FUICommandInfo> ExtractSprites;
	TSharedPtr<FUICommandInfo> ToggleShowRelatedSprites;
	TSharedPtr<FUICommandInfo> ToggleShowSpriteNames;

	// Editing modes
	TSharedPtr<FUICommandInfo> EnterViewMode;
	TSharedPtr<FUICommandInfo> EnterSourceRegionEditMode;
	TSharedPtr<FUICommandInfo> EnterCollisionEditMode;
	TSharedPtr<FUICommandInfo> EnterRenderingEditMode;

	// Timeline commands
	TSharedPtr<FUICommandInfo> AddKeyFrame;
	TSharedPtr<FUICommandInfo> AddNewFrame;
	TSharedPtr<FUICommandInfo> AddNewFrameBefore;
	TSharedPtr<FUICommandInfo> AddNewFrameAfter;

	// Asset commands
	TSharedPtr<FUICommandInfo> PickNewSpriteFrame;
	TSharedPtr<FUICommandInfo> EditSpriteFrame;
	TSharedPtr<FUICommandInfo> ShowInContentBrowser;
};
