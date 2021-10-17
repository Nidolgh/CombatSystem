// Copyright Epic Games, Inc. All Rights Reserved.

#include "SCombatFlipbookEditorViewportToolbar.h"

#include "SEditorViewport.h"

#include "CombatFlipbookEditor/CombatFlipbookEditorCommands.h"
#include "CombatFlipbookEditor/SpriteEditing/CombatSpriteGeometryEditCommands.h"

#define LOCTEXT_NAMESPACE "SSpriteEditorViewportToolbar"

void SCombatFlipbookEditorViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InInfoProvider);
}

TSharedRef<SWidget> SCombatFlipbookEditorViewportToolbar::GenerateShowMenu() const
{
	GetInfoProvider().OnFloatingButtonClicked();
	
	TSharedRef<SEditorViewport> ViewportRef = GetInfoProvider().GetViewportWidget();

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder ShowMenuBuilder(bInShouldCloseWindowAfterMenuSelection, ViewportRef->GetCommandList());
	{
		ShowMenuBuilder.AddMenuEntry(FCombatFlipbookEditorCommands::Get().SetShowSockets);
		ShowMenuBuilder.AddMenuEntry(FCombatFlipbookEditorCommands::Get().SetShowPivot);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FCombatFlipbookEditorCommands::Get().SetShowGrid);
		ShowMenuBuilder.AddMenuEntry(FCombatFlipbookEditorCommands::Get().SetShowBounds);
		ShowMenuBuilder.AddMenuEntry(FCombatSpriteGeometryEditCommands::Get().SetShowNormals);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FCombatFlipbookEditorCommands::Get().SetShowCollision);
		ShowMenuBuilder.AddMenuEntry(FCombatFlipbookEditorCommands::Get().SetShowMeshEdges);
	}

	return ShowMenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
