#pragma once

class FCombatFlipbookEditor;
struct FCombatFrameCollisionData;

class SFrameCollisionDataTab : public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SFrameCollisionDataTab) {}

	SLATE_END_ARGS()

		void Construct(const FArguments &InArgs, 
			TSharedPtr<FCombatFlipbookEditor> InFlipbookEditor);

	void Rebuild();

	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	// End of SWidget interface

	int32* GetButtonFrameID();

private:
	// Pointer back to owning sprite editor instance (the keeper of state)
	TWeakPtr<class FCombatFlipbookEditor> CombatFlipbookEditorPtr;
	TArray<FCombatFrameCollisionData>* KFArray;
	int32 LastInstructionNum = 0;
	int32 LastFrame = 0;

	// interaction
	int32 ButtonFrameInstructionID = 0;
	int32 LastButtonFrameInstructionID = 0;

	TSharedPtr<SVerticalBox> MainBoxPtr;
	TSharedPtr<SHorizontalBox> HorizontalButtonsPtr;
};