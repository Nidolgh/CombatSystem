// Copyright Epic Games, Inc. All Rights Reserved.

#include "SFlipbookDataEditorViewportToolbar.h"
#include "FlipbookDataEditorCommands.h"
#include "SpriteGeometryEditCommands.h"
#include "SEditorViewport.h"
#include "SpriteGeometryEditCommands.h"

#define LOCTEXT_NAMESPACE "SSpriteEditorViewportToolbar"

///////////////////////////////////////////////////////////
// SSpriteEditorViewportToolbar

void SFlipbookDataEditorViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InInfoProvider);
}

TSharedRef<SWidget> SFlipbookDataEditorViewportToolbar::GenerateShowMenu() const
{
	GetInfoProvider().OnFloatingButtonClicked();
	
	TSharedRef<SEditorViewport> ViewportRef = GetInfoProvider().GetViewportWidget();

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder ShowMenuBuilder(bInShouldCloseWindowAfterMenuSelection, ViewportRef->GetCommandList());
	{
		ShowMenuBuilder.AddMenuEntry(FFlipbookDataEditorCommands::Get().SetShowSockets);
		ShowMenuBuilder.AddMenuEntry(FFlipbookDataEditorCommands::Get().SetShowPivot);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FFlipbookDataEditorCommands::Get().SetShowGrid);
		ShowMenuBuilder.AddMenuEntry(FFlipbookDataEditorCommands::Get().SetShowBounds);
		ShowMenuBuilder.AddMenuEntry(FFlipbookDataGeometryEditCommands::Get().SetShowNormals);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FFlipbookDataEditorCommands::Get().SetShowCollision);
		ShowMenuBuilder.AddMenuEntry(FFlipbookDataEditorCommands::Get().SetShowMeshEdges);
	}

	return ShowMenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
