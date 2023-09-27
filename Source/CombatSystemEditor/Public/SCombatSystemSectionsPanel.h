#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "CoreMinimal.h"

class IPersonaToolkit;

struct FCombatSystemSectionsSummoner : public FWorkflowTabFactory
{
	FCombatSystemSectionsSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;
};

class SCombatSystemSectionsPanel : public SCompoundWidget, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS( SCombatSystemSectionsPanel )
	{}

	SLATE_END_ARGS()

	~SCombatSystemSectionsPanel();
	
	// FEditorUndoClient interface
	virtual void PostUndo(bool bSuccess) { Update(); };
	virtual void PostRedo(bool bSuccess) { Update(); };

	void Construct(const FArguments& InArgs);
	
	void ManageActiveEditMode(FEditorModeID ModeToActivate, FEditorModeID ModeToDeactivate);
	
	// Rebuild panel widgets
	void Update();

private:
	// Main panel area widget
	TSharedPtr<SBorder>	PanelArea;

	// Persona toolkit we are hosted in
	TWeakPtr<IPersonaToolkit> WeakPersonaToolkit;
};
