// Copyright Epic Games, Inc. All Rights Reserved.

#include "SC2DFrameInstructionsEditorViewportToolbar.h"
#include "C2DFrameInstructionsEditorCommands.h"
#include "SpriteGeometryEditCommands.h"
#include "SEditorViewport.h"
#include "SpriteGeometryEditCommands.h"

#define LOCTEXT_NAMESPACE "SSpriteEditorViewportToolbar"

///////////////////////////////////////////////////////////
// SSpriteEditorViewportToolbar

void SC2DFrameInstructionsEditorViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InInfoProvider);
}

TSharedRef<SWidget> SC2DFrameInstructionsEditorViewportToolbar::GenerateShowMenu() const
{
	GetInfoProvider().OnFloatingButtonClicked();
	
	TSharedRef<SEditorViewport> ViewportRef = GetInfoProvider().GetViewportWidget();

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder ShowMenuBuilder(bInShouldCloseWindowAfterMenuSelection, ViewportRef->GetCommandList());
	{
		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().SetShowSockets);
		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().SetShowPivot);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().SetShowGrid);
		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().SetShowBounds);
		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsGeometryEditCommands::Get().SetShowNormals);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().SetShowCollision);
		ShowMenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().SetShowMeshEdges);
	}

	return ShowMenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
