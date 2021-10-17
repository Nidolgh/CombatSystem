// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Combat2DStyle.h"

class FCombatSpriteGeometryEditCommands : public TCommands<FCombatSpriteGeometryEditCommands>
{
public:
	FCombatSpriteGeometryEditCommands()
		: TCommands<FCombatSpriteGeometryEditCommands>(
			TEXT("SpriteGeometryEditor"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "SpriteGeometryEditor", "Sprite Geometry Editor"), // Localized context name for displaying
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
	TSharedPtr<FUICommandInfo> SetShowNormals;

	// Geometry editing commands
	TSharedPtr<FUICommandInfo> DeleteSelection;
	TSharedPtr<FUICommandInfo> AddBoxShape;
	TSharedPtr<FUICommandInfo> AddCircleShape;
	TSharedPtr<FUICommandInfo> ToggleAddPolygonMode;
	TSharedPtr<FUICommandInfo> SnapAllVertices;
	TSharedPtr<FUICommandInfo> SetActiveGeometryStruct;
};
