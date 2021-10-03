#pragma once

#include "PaperFlipbookComponent.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

class FCombatFlipbookEditorViewportClient;
class FCombatFlipbookEditor;

class SCombatFlipbookEditorViewport
	: public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SCombatFlipbookEditorViewport)
	{
	}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<FCombatFlipbookEditor> InC2DFrameInstructionsEditor);

	// SEditorViewport interface
	virtual void BindCommands() override;
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	virtual EVisibility GetTransformToolbarVisibility() const override;
	virtual void OnFocusViewportToSelection() override;
	// End of SEditorViewport interface

	// ICommonEditorViewportToolbarInfoProvider interface
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;
	// End of ICommonEditorViewportToolbarInfoProvider interface

	void ShowExtractSpritesDialog();
	
	UPaperFlipbookComponent* GetPreviewComponent() const;

private:
	// Pointer back to owning C2DFrameInstructions editor instance (the keeper of state)
	TWeakPtr<FCombatFlipbookEditor> CombatFlipbookEditorPtr;

	// Viewport client
	TSharedPtr<FCombatFlipbookEditorViewportClient> EditorViewportClient;
};
