#pragma once

#include "CoreMinimal.h"


#include "MovesetEditor.h"

#include "ITransportControl.h"
#include "Combat2DEditor/Classes/Moveset.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"

//////////////////////////////////////////////////////////////////////////

class IMovesetEditor : public FAssetEditorToolkit {
	
public:
	
	virtual UMoveset* GetMovesetBeingEdited() = 0;

};

/**
 * 
 */
class COMBAT2DEDITOR_API FMovesetEditor : public IMovesetEditor
{
public:

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	/**
	 * Edits the specified asset object
	 *
	 * @param	Mode					Asset editing mode for this editor (standalone or world-centric)
	 * @param	InitToolkitHost			When Mode is WorldCentric, this is the level editor instance to spawn this editor within
	 * @param	InMoveset			The Custom Asset to Edit
	 */
	void InitMovesetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UMoveset* InMoveset);

	/** Destructor */
	virtual ~FMovesetEditor();

	/** Begin IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override { return true; }
	/** End IToolkit interface */

	/** Begin IMovesetEditor interface */
	virtual UMoveset* GetMovesetBeingEdited();
	/** End IMovesetEditor interface */

	void AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList = NULL, EExtensionHook::Position position = EExtensionHook::Before);
	TSharedRef<FWorkspaceItem> GetMenuRoot() const { return MenuRoot; };
	
protected:
	
	void ExtendToolbar();
	
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	
	void CreateModeToolbarWidgets(FToolBarBuilder& IgnoredBuilder);

	void MakePulldownMenu(FMenuBarBuilder &menuBuilder);
	void FillPulldownMenu(FMenuBuilder &menuBuilder);

	TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
	TSharedPtr<FExtender> MenuExtender;

	static TSharedRef<FWorkspaceItem> MenuRoot;

private:
	/** Create the properties tab and its content */
	TSharedRef<SDockTab> SpawnPropertiesTab(const FSpawnTabArgs& Args);

	/** Dockable tab for properties */
	TSharedPtr< SDockableTab > PropertiesTab;

	/** Details view */
	TSharedPtr<class IDetailsView> DetailsView;

	/** The Custom Asset open within this editor */
	UMoveset* MovesetBeingEdited = nullptr;
};