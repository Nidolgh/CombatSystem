#pragma once

#include "CoreMinimal.h"

#include "Combat2DEditor/Classes/C2DFrameInstructions.h"

#include "ITransportControl.h"
#include "PaperFlipbookComponent.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"

class SC2DFrameInstructionsEditorViewport;
class IDetailsView;
class SDockableTab;
class UC2DFrameInstructions;
class SGeometryPropertiesTabBody;

//////////////////////////////////////////////////////////////////////////
// 

namespace EC2DFrameInstructionsEditorMode
{
	enum Type
	{
		ViewMode,
		EditSourceRegionMode,
		EditCollisionMode,
		EditRenderingGeomMode
	};
}

//////////////////////////////////////////////////////////////////////////

class IC2DFrameInstructionsEditor : public FAssetEditorToolkit {
	
public:
	
	virtual UC2DFrameInstructions* GetC2DFrameInstructionsBeingEdited() = 0;

};

/**
 * 
 */
class COMBAT2DEDITOR_API FC2DFrameInstructionsEditor : public IC2DFrameInstructionsEditor
{
public:

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	/**
	 * Edits the specified asset object
	 *
	 * @param	Mode					Asset editing mode for this editor (standalone or world-centric)
	 * @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	 * @param	InC2DFrameInstructions			The Custom Asset to Edit
	 */
	void InitC2DFrameInstructionsEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UC2DFrameInstructions* InC2DFrameInstructions);

	/** Destructor */
	virtual ~FC2DFrameInstructionsEditor();

	UPaperFlipbookComponent* GetPreviewComponent() const;

	/** Begin IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override { return true; }
	/** End IToolkit interface */

	/** Begin IC2DFrameInstructionsEditor interface */
	virtual UC2DFrameInstructions* GetC2DFrameInstructionsBeingEdited();
	/** End IC2DFrameInstructionsEditor interface */

        UPaperFlipbook* GetFlipbookBeingEdited() const;
	
	int32 GetCurrentFrame() const;

	void AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList = NULL, EExtensionHook::Position position = EExtensionHook::Before);
	TSharedRef<FWorkspaceItem> GetMenuRoot() const { return MenuRoot; };

	SGeometryPropertiesTabBody* GetGeoPropTabBody() const { return GeometryPropertiesTabBody.Get(); }

	FC2DFrameInstructionsKeyFrame* CreateKeyFrameDataOnCurrentFrame();
	void CreateKeyFrameInstructionOnCurrentFrame();

protected:
	
	void ExtendToolbar();
	
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Geometry(const FSpawnTabArgs& Args);
	
	void CreateModeToolbarWidgets(FToolBarBuilder& IgnoredBuilder);

	void DeleteSelection();
	void DuplicateSelection();
	void SetSelection(int32 NewSelection);
	bool HasValidSelection() const;

	void AddKeyFrameAtCurrentTime();
	void AddNewKeyFrameAtEnd();
	void AddNewKeyFrameBefore();
	void AddNewKeyFrameAfter();

	FReply OnClick_Forward();
	FReply OnClick_Forward_Step();
	FReply OnClick_Forward_End();
	FReply OnClick_Backward();
	FReply OnClick_Backward_Step();
	FReply OnClick_Backward_End();
	FReply OnClick_ToggleLoop();

	uint32 GetTotalFrameCount() const;
	uint32 GetTotalFrameCountPlusOne() const;
	float GetTotalSequenceLength() const;
	float GetPlaybackPosition() const;
	void SetPlaybackPosition(float NewTime);
	bool IsLooping() const;
	EPlaybackMode::Type GetPlaybackMode() const;

	float GetViewRangeMin() const;
	float GetViewRangeMax() const;
	void SetViewRange(float NewMin, float NewMax);

	float GetFramesPerSecond() const;
	void SetCurrentFrame(int32 NewIndex);

	void MakePulldownMenu(FMenuBarBuilder &menuBuilder);
	void FillPulldownMenu(FMenuBuilder &menuBuilder);

	TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
	TSharedPtr<FExtender> MenuExtender;

	static TSharedRef<FWorkspaceItem> MenuRoot;

	// Selection set
	int32 CurrentSelectedKeyframe;

	// Range of times currently being viewed
	mutable float ViewInputMin;
	mutable float ViewInputMax;
	mutable float LastObservedSequenceLength;

private:
	/** Create the properties tab and its content */
	TSharedRef<SDockTab> SpawnPropertiesTab(const FSpawnTabArgs& Args);

	/** Dockable tab for properties */
	TSharedPtr< SDockableTab > PropertiesTab;

	/** Details view */
	TSharedPtr<class IDetailsView> DetailsView;

	/** The Custom Asset open within this editor */
	UC2DFrameInstructions* C2DFrameInstructionsBeingEdited = nullptr;
	TSharedPtr<SC2DFrameInstructionsEditorViewport> ViewportPtr;

	TSharedPtr<SGeometryPropertiesTabBody> GeometryPropertiesTabBody;
};

/////////////////////////////////////////////////////
// SGeometryPropertiesTabBody

class SGeometryPropertiesTabBody : public SCompoundWidget {
public:

	SLATE_BEGIN_ARGS(SGeometryPropertiesTabBody) {
	}

	SLATE_END_ARGS()

		void Construct(const FArguments &InArgs, 
			TSharedPtr<FC2DFrameInstructionsEditor> InFlipbookEditor)
	{
		C2DFrameInstructionsEditorPtr = InFlipbookEditor;

		ChildSlot
			[
				SAssignNew(MainBoxPtr, SVerticalBox)
			];

		Rebuild();
	}

	void Rebuild()
	{
		UC2DFrameInstructions* C2DFrameInstructions = C2DFrameInstructionsEditorPtr.Pin()->GetC2DFrameInstructionsBeingEdited();
		if (C2DFrameInstructions->KeyFrameArray.Num() == 0)
		{
			return;
		}
		
		KFArray = &C2DFrameInstructions->KeyFrameArray[C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame()].KeyFrameInstructions;

		MainBoxPtr->ClearChildren();
		//HorizontalButtonsPtr->ClearChildren();

		FString HeaderBase = TEXT("Active instruction frame");
		HeaderBase.Append("_");
		HeaderBase.Append(FString::FromInt(C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame()));
		HeaderBase.Append("-");
		HeaderBase.Append(FString::FromInt(ButtonFrameInstructionID));

		if (KFArray->Num() == 0)
		{
			HeaderBase = TEXT("No instruction frames");
		}
		
		MainBoxPtr->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(HeaderBase))
			];
		MainBoxPtr->AddSlot()
			.AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Add Instruction")))
				.OnClicked_Lambda([this]()
					{
						C2DFrameInstructionsEditorPtr.Pin().Get()->CreateKeyFrameInstructionOnCurrentFrame();

						return FReply::Handled();
					})
			];
		MainBoxPtr->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Instructions:"))
			];
		MainBoxPtr->AddSlot()
			.AutoHeight()[
				SAssignNew(HorizontalButtonsPtr, SHorizontalBox)
			];
		MainBoxPtr->AddSlot()
			.AutoHeight()[
				SNew(SScrollBar)
			];
		
		if (KFArray != nullptr)
		{
			for (size_t i = 0; i < KFArray->Num(); i++)
			{
				FString ButtonText = TEXT("");
				ButtonText.Append(FString::FromInt(i));

				FLinearColor ButtonCol(0.5f, 0.5f, 0.5f, 1.f);
				if (i == ButtonFrameInstructionID)
				{
					ButtonCol = FLinearColor::White;
				}
				
				HorizontalButtonsPtr.Get()->AddSlot()
					.AutoWidth()
					[
						SNew(SButton)
						.ClickMethod(EButtonClickMethod::MouseDown)
						.Text(FText::FromString(ButtonText))
						.ButtonColorAndOpacity(FSlateColor(ButtonCol))
						.OnClicked_Lambda([&, i]()
						{
							UE_LOG(LogTemp, Log, TEXT("Button pressed: %i"), i);

							ButtonFrameInstructionID = i;

							return FReply::Handled();
						})
					];
			}

			MainBoxPtr->AddSlot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Type:"))
				];

			InstructionType& InstructionTypeRef = KFArray->GetData()[ButtonFrameInstructionID].InstructionType;
			
			for (size_t i = 0; i < static_cast<size_t>(InstructionType::InstructionTypeCount); i++)
			{
				FString InsTypeString("");
				FLinearColor InsTypeLColor(FLinearColor::White);
				const InstructionType CurType = static_cast<InstructionType>(i);
				
				switch (CurType)
				{
				case InstructionType::HitBox:
					InsTypeString = TEXT("HitBox");
					break;
				case InstructionType::HurtBox:
					InsTypeString = TEXT("HurtBox");
					break;
				default: 
					InsTypeString = TEXT("Type undefined!");
				}

				if (CurType != InstructionTypeRef)
				{
					InsTypeLColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.f);
				}
				
				MainBoxPtr->AddSlot()
					.AutoHeight()[
						SNew(SButton)
							.Text(FText::FromString(InsTypeString))
							.ButtonColorAndOpacity(FSlateColor(InsTypeLColor))
							.OnClicked_Lambda([this, i, &InstructionTypeRef]() mutable
							{
								InstructionTypeRef = static_cast<InstructionType>(i);

								Rebuild();
								
								return FReply::Handled();
							})
					];
			}
		}
	}

	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
	{
		// check for rebuild C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame()
		if ( KFArray != nullptr
			&& (( LastInstructionNum != KFArray->Num() ) 
			|| ( LastFrame != C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame() )
			|| ( LastButtonFrameInstructionID != ButtonFrameInstructionID ) )
			) 
		{
			LastButtonFrameInstructionID = ( LastFrame == C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame() ) ? ButtonFrameInstructionID : 0;

			LastInstructionNum = KFArray->Num();
			LastFrame = C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame();

			Rebuild();
		}
		else if (KFArray == nullptr && C2DFrameInstructionsEditorPtr.Pin()->GetC2DFrameInstructionsBeingEdited()->KeyFrameArray.Num() > 0)
		{
			KFArray = &C2DFrameInstructionsEditorPtr.Pin()->GetC2DFrameInstructionsBeingEdited()->KeyFrameArray[C2DFrameInstructionsEditorPtr.Pin()->GetCurrentFrame()].KeyFrameInstructions;
		}
	}
	// End of SWidget interface

	int32* GetButtonFrameID()
	{
		return &ButtonFrameInstructionID;
	}

private:
	// Pointer back to owning sprite editor instance (the keeper of state)
	TWeakPtr<class FC2DFrameInstructionsEditor> C2DFrameInstructionsEditorPtr;
	TArray<FC2DFrameInstructionsKeyFrameInstruction>* KFArray;
	int32 LastInstructionNum = 0;
	int32 LastFrame = 0;

	// interaction
	int32 ButtonFrameInstructionID = 0;
	int32 LastButtonFrameInstructionID = 0;

	TSharedPtr<SVerticalBox> MainBoxPtr;
	TSharedPtr<SHorizontalBox> HorizontalButtonsPtr;
};