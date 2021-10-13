#include "CombatFlipbookEditorViewport.h"

#include "CombatFlipbookEditor.h"
#include "CombatFlipbookEditorCommands.h"
#include "CombatFlipbookEditorViewportClient.h"

#include "SpriteEditing/SpriteGeometryEditCommands.h"
#include "SpriteEditing/SpriteGeometryEditMode.h"

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

	const FSpriteGeometryEditCommands& Commands2 = FSpriteGeometryEditCommands::Get();
	const TSharedRef<FSpriteGeometryEditMode> GeometryEditRef = EditorViewportClientRef.Get().GeometryEditMode.
		ToSharedRef();
	FSpriteGeometryEditingHelper* GeometryHelper = GeometryEditRef->GetGeometryHelper();

	CommandList->MapAction(
		Commands2.AddBoxShape,
		FExecuteAction::CreateSP(EditorViewportClientRef.Get().GeometryEditMode.ToSharedRef(),
			&FSpriteGeometryEditMode::AddBoxShape),
		FCanExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddBoxShape),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddBoxShape));

	// Show toggles
	CommandList->MapAction(
		Commands2.SetShowNormals,
		FExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::ToggleShowNormals),
		FCanExecuteAction(),
		FIsActionChecked::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::IsShowNormalsEnabled));

	//// Geometry editing commands
	CommandList->MapAction(
		Commands2.DeleteSelection,
		FExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::DeleteSelectedItems),
		FCanExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanDeleteSelection));

	CommandList->MapAction(
		Commands2.AddBoxShape,
		FExecuteAction::CreateSP(GeometryEditRef, &FSpriteGeometryEditMode::AddBoxShape),
		FCanExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddBoxShape),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddBoxShape));

	CommandList->MapAction(
		Commands2.ToggleAddPolygonMode,
		FExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::ToggleAddPolygonMode),
		FCanExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddPolygon),
		FIsActionChecked::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::IsAddingPolygon),
		FIsActionButtonVisible::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddPolygon));

	CommandList->MapAction(
		Commands2.AddCircleShape,
		FExecuteAction::CreateSP(GeometryEditRef, &FSpriteGeometryEditMode::AddCircleShape),
		FCanExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddCircleShape),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanAddCircleShape));

	CommandList->MapAction(
		Commands2.SnapAllVertices,
		FExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::SnapAllVerticesToPixelGrid),
		FCanExecuteAction::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanSnapVerticesToPixelGrid),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateRaw(GeometryHelper, &FSpriteGeometryEditingHelper::CanSnapVerticesToPixelGrid));
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
