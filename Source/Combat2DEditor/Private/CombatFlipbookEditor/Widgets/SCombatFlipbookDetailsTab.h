#pragma once

#include "SSingleObjectDetailsPanel.h"

class FCombatFlipbookEditor;
class FCombatFlipbookEditor;

class SCombatFlipbookDetailsTab : public SSingleObjectDetailsPanel
{
public:
SLATE_BEGIN_ARGS(SCombatFlipbookDetailsTab)
		{
		}

	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs,
	               TSharedPtr<FCombatFlipbookEditor> InFlipbookEditor);

	// SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override;

	virtual TSharedRef<SWidget> PopulateSlot(
		TSharedRef<SWidget> PropertyEditorWidget) override;
	// End of SSingleObjectDetailsPanel interface

private:
	// Pointer back to owning sprite editor instance (the keeper of state)
	TWeakPtr<FCombatFlipbookEditor> CombatFlipbookEditorPtr;
};
