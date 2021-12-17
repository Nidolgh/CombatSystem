#pragma once

#include "CoreMinimal.h"

#include "CombatFlipbook.h"
#include "ITransportControl.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"

class IDetailsView;
class SDockableTab;
class SFrameCollisionDataTab;
class UPaperFlipbook;
class UPaperFlipbookComponent;
class SCombatFlipbookEditorViewport;

/**
* 
*/
class COMBAT2DEDITOR_API FCombatFlipbookEditor : public FAssetEditorToolkit
{
public:

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	/**
	* Edits the specified asset object
	*
	* @param	Mode					Asset editing mode for this editor (standalone or world-centric)
	* @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	* @param	InCustomAsset			The Custom Asset to Edit
	*/
	void InitCombatFlipbookEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UCombatFlipbook* InCustomAsset);

	/** Destructor */
	virtual ~FCombatFlipbookEditor();

	/** Begin IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override { return true; }
	/** End IToolkit interface */

	virtual UCombatFlipbook* GetCombatFlipbookBeingEdited();
	UPaperFlipbookComponent* GetPreviewComponent() const;
	UPaperFlipbook* GetFlipbookBeingEdited() const;


	void AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList = NULL, EExtensionHook::Position position = EExtensionHook::Before);
	TSharedRef<FWorkspaceItem> GetMenuRoot() const { return MenuRoot; };

	SFrameCollisionDataTab* GetGeoPropTabBody() const { return FrameCollisionDataTab.Get(); }

	FCombatFrame* CreateKeyFrameDataOnCurrentFrame();
	void CreateKeyFrameInstructionOnCurrentFrame();
	int32 GetCurrentFrame() const;

	void ActivateEditMode();
	
public:
	/** The name given to all instances of this type of editor */
	static const FName ToolkitFName;
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
	UCombatFlipbook* CombatFlipbookBeingEdited = nullptr;
	TSharedPtr<SCombatFlipbookEditorViewport> ViewportPtr;

	TSharedPtr<SFrameCollisionDataTab> FrameCollisionDataTab;
};