#include "CombatFlipbookEditorViewport.h"

#include "CombatFlipbookEditor.h"
#include "CombatFlipbookEditorCommands.h"
#include "CombatFlipbookEditorViewportClient.h"

#include "SpriteEditing/CombatSpriteGeometryEditCommands.h"
#include "SpriteEditing/CombatSpriteGeometryEditMode.h"

#include "Widgets/SCombatFlipbookEditorViewportToolbar.h"

//////////////////////////////////////////////////////////////////////////
// SCombatFlipbookViewport

void SCombatFlipbookEditorViewport::Construct(const FArguments& InArgs,
                                        TSharedPtr<FCombatFlipbookEditor> InCombatFlipbookEditor)
{
	CombatFlipbookEditorPtr = InCombatFlipbookEditor;
	
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SCombatFlipbookEditorViewport::BindCommands()
{
	SEditorViewport::BindCommands();

	const FCombatFlipbookEditorCommands& Commands =
		FCombatFlipbookEditorCommands::Get();

	TSharedRef<FCombatFlipbookEditorViewportClient> EditorViewportClientRef = EditorViewportClient.ToSharedRef();

	// Show toggles
	CommandList->MapAction(
		Commands.SetShowGrid,
		FExecuteAction::CreateSP(EditorViewportClientRef,
			&FEditorViewportClient::SetShowGrid),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(EditorViewportClientRef,
			&FEditorViewportClient::IsSetShowGridChecked));

	CommandList->MapAction(
		Commands.SetShowBounds,
		FExecuteAction::CreateSP(EditorViewportClientRef,
			&FEditorViewportClient::ToggleShowBounds),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(EditorViewportClientRef,
			&FEditorViewportClient::IsSetShowBoundsChecked));

	CommandList->MapAction(
		Commands.SetShowCollision,
		FExecuteAction::CreateSP(EditorViewportClientRef, &FEditorViewportClient::SetShowCollision),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(EditorViewportClientRef, &FEditorViewportClient::IsSetShowCollisionChecked));

	CommandList->MapAction(
		Commands.SetShowPivot,
		FExecuteAction::CreateSP(EditorViewportClientRef,
			&FCombatFlipbookEditorViewportClient::ToggleShowPivot),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(EditorViewportClientRef,
			&FCombatFlipbookEditorViewportClient::IsShowPivotChecked));
}

TSharedRef<FEditorViewportClient>
SCombatFlipbookEditorViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(
		new FCombatFlipbookEditorViewportClient(CombatFlipbookEditorPtr,
			SharedThis(this)));

	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SCombatFlipbookEditorViewport::MakeViewportToolbar()
{
	return SNew(SCombatFlipbookEditorViewportToolbar, SharedThis(this));
}

EVisibility SCombatFlipbookEditorViewport::GetTransformToolbarVisibility() const
{
	return EVisibility::Visible;
}

void SCombatFlipbookEditorViewport::OnFocusViewportToSelection()
{
	//EditorViewportClient->RequestFocusOnSelection(/*bInstant=*/ false);
}

TSharedRef<class SEditorViewport>
SCombatFlipbookEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SCombatFlipbookEditorViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SCombatFlipbookEditorViewport::OnFloatingButtonClicked()
{
}

void SCombatFlipbookEditorViewport::ShowExtractSpritesDialog()
{
	/*if (UPaperSprite* Sprite = SpriteEditorPtr.Pin()->GetSpriteBeingEdited())
	{
	if (UTexture2D* SourceTexture = Sprite->GetSourceTexture())
	{
	SPaperExtractSpritesDialog::ShowWindow(SourceTexture);
	}
	}*/
}

UPaperFlipbookComponent*
SCombatFlipbookEditorViewport::GetPreviewComponent() const
{
	return EditorViewportClient->GetPreviewComponent();
}

void SCombatFlipbookEditorViewport::ActivateEditMode()
{
	EditorViewportClient->ActivateEditMode();
}

//////////////////////////////////////////////////////////////////////////
