#include "C2DFrameInstructionsEditor.h"

#include "CanvasItem.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"

#include "Combat2DModule.h"

#include "Combat2DEditor/Classes/C2DFrameInstructions.h"
#include "C2DFrameInstructionsEditorCommands.h"
#include "C2DFrameInstructionsEditorViewportClient.h"
#include "SpriteGeometryEditCommands.h"
#include "SC2DFrameInstructionsEditorViewportToolbar.h"
#include "SScrubControlPanel.h"
#include "SC2DFrameInstructionsTimeline.h"
#include "SCommonEditorViewportToolbarBase.h"

#include "SEditorViewport.h"
#include "SSingleObjectDetailsPanel.h"

#define LOCTEXT_NAMESPACE "C2DFrameInstructionsEditor"

//////////////////////////////////////////////////////////////////////////

const FName C2DFrameInstructionsEditorAppName = FName(TEXT("C2DFrameInstructionsEditorApp"));

TSharedRef<FWorkspaceItem> FC2DFrameInstructionsEditor::MenuRoot = FWorkspaceItem::NewGroup(
	FText::FromString("Menu Root"));

struct FC2DFrameInstructionsEditorTabs
{
	// Tab identifiers
	static const FName DetailsID;
	static const FName ViewportID;
	static const FName GeometryID;
};

const FName FC2DFrameInstructionsEditorTabs::DetailsID(TEXT("Details"));
const FName FC2DFrameInstructionsEditorTabs::ViewportID(TEXT("Viewport"));
const FName FC2DFrameInstructionsEditorTabs::GeometryID(TEXT("Geometry"));

/////////////////////////////////////////////////////
// SC2DFrameInstructionsPropertiesTabBody

class SC2DFrameInstructionsPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
SLATE_BEGIN_ARGS(SC2DFrameInstructionsPropertiesTabBody)
		{
		}

	SLATE_END_ARGS()

private:
	// Pointer back to owning sprite editor instance (the keeper of state)
	TWeakPtr<class FC2DFrameInstructionsEditor> C2DFrameInstructionsEditorPtr;
public:
	void Construct(const FArguments& InArgs,
	               TSharedPtr<FC2DFrameInstructionsEditor> InFlipbookEditor)
	{
		C2DFrameInstructionsEditorPtr = InFlipbookEditor;

		SSingleObjectDetailsPanel::Construct(
			SSingleObjectDetailsPanel::FArguments().
			HostCommandList(InFlipbookEditor->GetToolkitCommands()).
			HostTabManager(InFlipbookEditor->GetTabManager()),
			/*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/
			true);
	}

	// SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override
	{
		return C2DFrameInstructionsEditorPtr.Pin()->GetC2DFrameInstructionsBeingEdited();
	}

	virtual TSharedRef<SWidget> PopulateSlot(
		TSharedRef<SWidget> PropertyEditorWidget) override
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				PropertyEditorWidget
			];
	}

	// End of SSingleObjectDetailsPanel interface
};

//////////////////////////////////////////////////////////////////////////
// SC2DFrameInstructionsViewport

class SC2DFrameInstructionsEditorViewport
	: public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
SLATE_BEGIN_ARGS(SC2DFrameInstructionsEditorViewport)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs,
	               TSharedPtr<FC2DFrameInstructionsEditor> InC2DFrameInstructionsEditor);

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
	TWeakPtr<class FC2DFrameInstructionsEditor> C2DFrameInstructionsEditorPtr;

	// Viewport client
	TSharedPtr<FC2DFrameInstructionsEditorViewportClient> EditorViewportClient;
};

void SC2DFrameInstructionsEditorViewport::Construct(const FArguments& InArgs,
                                                    TSharedPtr<FC2DFrameInstructionsEditor>
                                                    InC2DFrameInstructionsEditor)
{
	C2DFrameInstructionsEditorPtr = InC2DFrameInstructionsEditor;

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SC2DFrameInstructionsEditorViewport::BindCommands()
{
	SEditorViewport::BindCommands();

	const FC2DFrameInstructionsEditorCommands& Commands =
		FC2DFrameInstructionsEditorCommands::Get();

	TSharedRef<FC2DFrameInstructionsEditorViewportClient> EditorViewportClientRef = EditorViewportClient.ToSharedRef();


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
		                         &FC2DFrameInstructionsEditorViewportClient::ToggleShowPivot),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(EditorViewportClientRef,
		                           &FC2DFrameInstructionsEditorViewportClient::IsShowPivotChecked));


	const FC2DFrameInstructionsGeometryEditCommands& Commands2 = FC2DFrameInstructionsGeometryEditCommands::Get();
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
SC2DFrameInstructionsEditorViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(
		new FC2DFrameInstructionsEditorViewportClient(C2DFrameInstructionsEditorPtr,
		                                              SharedThis(this)));

	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SC2DFrameInstructionsEditorViewport::MakeViewportToolbar()
{
	return SNew(SC2DFrameInstructionsEditorViewportToolbar, SharedThis(this));
}

EVisibility SC2DFrameInstructionsEditorViewport::GetTransformToolbarVisibility() const
{
	return EVisibility::Visible;
}

void SC2DFrameInstructionsEditorViewport::OnFocusViewportToSelection()
{
	//EditorViewportClient->RequestFocusOnSelection(/*bInstant=*/ false);
}

TSharedRef<class SEditorViewport>
SC2DFrameInstructionsEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SC2DFrameInstructionsEditorViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SC2DFrameInstructionsEditorViewport::OnFloatingButtonClicked()
{
}

void SC2DFrameInstructionsEditorViewport::ShowExtractSpritesDialog()
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
SC2DFrameInstructionsEditorViewport::GetPreviewComponent() const
{
	return EditorViewportClient->GetPreviewComponent();
}

//////////////////////////////////////////////////////////////////////////

void FC2DFrameInstructionsEditor::RegisterTabSpawners(
	const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
		LOCTEXT("WorkspaceMenu_C2DFrameInstructionsEditor", "Flipbook Data Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FC2DFrameInstructionsEditorTabs::ViewportID,
	                                 FOnSpawnTab::CreateSP(
		                                 this,
		                                 &FC2DFrameInstructionsEditor::SpawnTab_Viewport))
	            .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
	                                "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FC2DFrameInstructionsEditorTabs::DetailsID,
	                                 FOnSpawnTab::CreateSP(
		                                 this,
		                                 &FC2DFrameInstructionsEditor::SpawnTab_Details))
	            .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
	                                "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FC2DFrameInstructionsEditorTabs::GeometryID,
	                                 FOnSpawnTab::CreateSP(
		                                 this,
		                                 &FC2DFrameInstructionsEditor::SpawnTab_Geometry))
	            .SetDisplayName(LOCTEXT("GeometryTab", "Geometry"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef())
	            .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
	                                "LevelEditor.Tabs.Geometry"));
}

void FC2DFrameInstructionsEditor::UnregisterTabSpawners(
	const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FC2DFrameInstructionsEditorTabs::DetailsID);
}

void FC2DFrameInstructionsEditor::InitC2DFrameInstructionsEditor(
	const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	UC2DFrameInstructions* InC2DFrameInstructions)
{
	C2DFrameInstructionsBeingEdited = InC2DFrameInstructions;

	FC2DFrameInstructionsEditorCommands::Register();
	FC2DFrameInstructionsGeometryEditCommands::Register();

	TSharedPtr<FC2DFrameInstructionsEditor> C2DFrameInstructionsEditorPtr = SharedThis(this);
	ViewportPtr = SNew(SC2DFrameInstructionsEditorViewport, C2DFrameInstructionsEditorPtr);

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout =
		FTabManager::NewLayout("Standalone_FlipbookAssetEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.2f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.75f)
					->SetHideTabWell(true)
					->AddTab(FC2DFrameInstructionsEditorTabs::ViewportID,
					         ETabState::OpenedTab)
				)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(FC2DFrameInstructionsEditorTabs::DetailsID,
					         ETabState::OpenedTab)
				)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(FC2DFrameInstructionsEditorTabs::DetailsID,
					         ETabState::OpenedTab)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		C2DFrameInstructionsEditorAppIdentifier,
		StandaloneDefaultLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		reinterpret_cast<UObject*>(InC2DFrameInstructions));

	ExtendToolbar();
	RegenerateMenusAndToolbars();
}

FC2DFrameInstructionsEditor::~FC2DFrameInstructionsEditor()
{
	DetailsView.Reset();
	PropertiesTab.Reset();
}

UPaperFlipbookComponent* FC2DFrameInstructionsEditor::GetPreviewComponent() const
{
	UPaperFlipbookComponent* PreviewComponent = ViewportPtr->
		GetPreviewComponent();
	check(PreviewComponent);
	return PreviewComponent;
}

FName FC2DFrameInstructionsEditor::GetToolkitFName() const
{
	return C2DFrameInstructionsEditorAppName;
}

FText FC2DFrameInstructionsEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Flipbook Data Editor");
}

FText FC2DFrameInstructionsEditor::GetToolkitToolTipText() const
{
	return LOCTEXT("ToolTip", "Flipbook Data Editor");
}

FString FC2DFrameInstructionsEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString();
}

FLinearColor FC2DFrameInstructionsEditor::GetWorldCentricTabColorScale() const
{
	return FColor::Red;
}

UC2DFrameInstructions* FC2DFrameInstructionsEditor::GetC2DFrameInstructionsBeingEdited()
{
	return C2DFrameInstructionsBeingEdited;
}

UPaperFlipbook* FC2DFrameInstructionsEditor::GetFlipbookBeingEdited() const
{
	return C2DFrameInstructionsBeingEdited->TargetFlipbook;
}

FC2DFrameInstructionsKeyFrame* FC2DFrameInstructionsEditor::CreateKeyFrameDataOnCurrentFrame()
{
	const int32 curFrame = GetCurrentFrame();

	TArray<FC2DFrameInstructionsKeyFrame>& keyFrameArray = GetC2DFrameInstructionsBeingEdited()->KeyFrameArray;

	while (!keyFrameArray.IsValidIndex(curFrame))
	{
		if (curFrame == INDEX_NONE)
		{
			return nullptr;
		}

		const int32 index = keyFrameArray.AddDefaulted();

		// add one frame instruction just to be sure
		keyFrameArray[index].KeyFrameInstructions.AddDefaulted();
	}

	return &keyFrameArray[curFrame];
}

void FC2DFrameInstructionsEditor::CreateKeyFrameInstructionOnCurrentFrame()
{
	const int32 curFrame = GetCurrentFrame();

	if (curFrame == INDEX_NONE)
	{
		return;
	}

	TArray<FC2DFrameInstructionsKeyFrame>& keyFrameArray = GetC2DFrameInstructionsBeingEdited()->KeyFrameArray;

	if (keyFrameArray.IsValidIndex(curFrame))
	{
		keyFrameArray[curFrame].KeyFrameInstructions.AddDefaulted();
	}
}

void FC2DFrameInstructionsEditor::ExtendToolbar()
{
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			const FC2DFrameInstructionsGeometryEditCommands& GeometryCommands =
				FC2DFrameInstructionsGeometryEditCommands::Get();
			ToolbarBuilder.BeginSection("Tools");
			{
				ToolbarBuilder.AddToolBarButton(GeometryCommands.AddBoxShape);
				ToolbarBuilder.AddToolBarButton(GeometryCommands.ToggleAddPolygonMode);
				ToolbarBuilder.AddToolBarButton(GeometryCommands.AddCircleShape);
				ToolbarBuilder.AddToolBarButton(GeometryCommands.SnapAllVertices);
			}
			ToolbarBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		ViewportPtr->GetCommandList(),
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar)
	);

	AddToolbarExtender(ToolbarExtender);

	/*if (!IsRunningCommandlet())
	{
	    ICombat2DModule& Combat2DModule = FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
	    LevelEditorMenuExtensibilityManager = Combat2DModule.GetMenuExtensibilityManager();
	    MenuExtender = MakeShareable(new FExtender);
	    MenuExtender->AddMenuBarExtension("Flipbook Data Editor", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FC2DFrameInstructionsEditor::MakePulldownMenu));
	    LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}*/

	//ICombat2DModule* Combat2DModule = &FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
	//AddToolbarExtender(Combat2DModule->GetSpriteEditorToolBarExtensibilityManager()->GetAllExtenders());
}

TSharedRef<SDockTab> FC2DFrameInstructionsEditor::SpawnTab_Viewport(
	const FSpawnTabArgs& Args)
{
	const FText text = LOCTEXT("EditCollisionGeometry_CornerText",
	                           "Edit Collision");

	FText test = LOCTEXT("EditCollisionGeometry_CornerText", "Edit Collision");

	ViewInputMin = 0.0f;
	ViewInputMax = GetFlipbookBeingEdited() == nullptr ? 0.f : GetTotalSequenceLength();
	LastObservedSequenceLength = ViewInputMax;

	TSharedRef<SWidget> ScrubControl = SNew(SScrubControlPanel)
                                     .IsEnabled(true)
                                     .Value(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::GetPlaybackPosition)
                                     .NumOfKeys(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::GetTotalFrameCountPlusOne)
                                     .SequenceLength(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::GetTotalSequenceLength)
                                     .OnValueChanged(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::SetPlaybackPosition)
                                     //		.OnBeginSliderMovement(this, &SAnimationScrubPanel::OnBeginSliderMovement)
                                     //		.OnEndSliderMovement(this, &SAnimationScrubPanel::OnEndSliderMovement)
                                     .OnClickedForwardPlay(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_Forward)
                                     .OnClickedForwardStep(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_Forward_Step)
                                     .OnClickedForwardEnd(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_Forward_End)
                                     .OnClickedBackwardPlay(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_Backward)
                                     .OnClickedBackwardStep(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_Backward_Step)
                                     .OnClickedBackwardEnd(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_Backward_End)
                                     .OnClickedToggleLoop(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::OnClick_ToggleLoop)
                                     .OnGetLooping(
		                                                           this, &FC2DFrameInstructionsEditor::IsLooping)
                                     .OnGetPlaybackMode(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::GetPlaybackMode)
                                     .ViewInputMin(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::GetViewRangeMin)
                                     .ViewInputMax(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::GetViewRangeMax)
                                     .OnSetInputViewRange(
		                                                           this,
		                                                           &FC2DFrameInstructionsEditor::SetViewRange)
                                     .bAllowZoom(true)
                                     .IsRealtimeStreamingMode(false);

	return SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"))
		[
			SNew(SVerticalBox)

			// The sprite editor viewport
			+ SVerticalBox::Slot()
			[
				ViewportPtr.ToSharedRef()
			]
			// Bottom-right corner text indicating the preview nature of the sprite editor
			+ SVerticalBox::Slot()
			  .Padding(0, 8, 0, 0)
			  .AutoHeight()
			[
				SNew(SC2DFrameInstructionsTimeline, GetToolkitCommands())
                 .C2DFrameInstructionsBeingEdited(GetC2DFrameInstructionsBeingEdited())
                 .OnSelectionChanged(this, &FC2DFrameInstructionsEditor::SetSelection)
                 .PlayTime(this, &FC2DFrameInstructionsEditor::GetPlaybackPosition)
			]

			+ SVerticalBox::Slot()
			  .Padding(0, 8, 0, 0)
			  .AutoHeight()
			[
				ScrubControl
			]
			/*+ SOverlay::Slot()
			      .Padding(10)
			      .VAlign(VAlign_Bottom)
			      .HAlign(HAlign_Right)
			[
			      SNew(STextBlock)
		   .Visibility(EVisibility::HitTestInvisible)
			   .TextStyle(FEditorStyle::Get(), "Graph.CornerText")
			   .Text(test)
			]*/
		];
}

TSharedRef<SDockTab> FC2DFrameInstructionsEditor::SpawnTab_Details(
	const FSpawnTabArgs& Args)
{
	TSharedPtr<FC2DFrameInstructionsEditor> FlipbookEditorPtr = SharedThis(this);

	// Spawn the tab
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
	[
		SNew(SC2DFrameInstructionsPropertiesTabBody, FlipbookEditorPtr)
	];
}

TSharedRef<SDockTab> FC2DFrameInstructionsEditor::SpawnTab_Geometry(
	const FSpawnTabArgs& Args)
{
	TSharedPtr<FC2DFrameInstructionsEditor> FlipbookEditorPtr = SharedThis(this);

	GeometryPropertiesTabBody = SNew(SGeometryPropertiesTabBody, FlipbookEditorPtr);

	// Spawn the tab
	return SNew(SDockTab)
        .Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Geometry"))
        .Label(LOCTEXT("GeometryTab_Title", "Geometry"))
	[
		GeometryPropertiesTabBody.ToSharedRef()
	];
}

void FC2DFrameInstructionsEditor::CreateModeToolbarWidgets(
	FToolBarBuilder& IgnoredBuilder)
{
	FToolBarBuilder ToolbarBuilder(ViewportPtr->GetCommandList(),
	                               FMultiBoxCustomization::None);
	ToolbarBuilder.AddToolBarButton(
		FC2DFrameInstructionsEditorCommands::Get().EnterViewMode);
	ToolbarBuilder.AddToolBarButton(
		FC2DFrameInstructionsEditorCommands::Get().EnterSourceRegionEditMode);
	ToolbarBuilder.AddToolBarButton(
		FC2DFrameInstructionsEditorCommands::Get().EnterCollisionEditMode);
	ToolbarBuilder.AddToolBarButton(
		FC2DFrameInstructionsEditorCommands::Get().EnterRenderingEditMode);
	AddToolbarWidget(ToolbarBuilder.MakeWidget());
}

TSharedRef<SDockTab> FC2DFrameInstructionsEditor::SpawnPropertiesTab(
	const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FC2DFrameInstructionsEditorTabs::DetailsID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
	[
		DetailsView.ToSharedRef()
	];
}

void FC2DFrameInstructionsEditor::AddKeyFrameAtCurrentTime()
{
	const FScopedTransaction Transaction(
		LOCTEXT("InsertKeyFrameTransaction", "Insert Key Frame"));
	C2DFrameInstructionsBeingEdited->Modify();

	const float CurrentTime = GetPlaybackPosition();
	const int32 KeyFrameIndex = GetFlipbookBeingEdited()->GetKeyFrameIndexAtTime(
		CurrentTime);
	const int32 ClampedIndex = FMath::Clamp<int32>(
		KeyFrameIndex, 0, GetFlipbookBeingEdited()->GetNumFrames());

	FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());
	FPaperFlipbookKeyFrame NewFrame;
	EditLock.KeyFrames.Insert(NewFrame, ClampedIndex);
}

void FC2DFrameInstructionsEditor::AddNewKeyFrameAtEnd()
{
	const FScopedTransaction Transaction(
		LOCTEXT("AddKeyFrameTransaction", "Add Key Frame"));
	GetFlipbookBeingEdited()->Modify();

	FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());

	FPaperFlipbookKeyFrame& NewFrame = *new(EditLock.KeyFrames)
		FPaperFlipbookKeyFrame();
}

void FC2DFrameInstructionsEditor::AddNewKeyFrameBefore()
{
	if (GetFlipbookBeingEdited()->IsValidKeyFrameIndex(CurrentSelectedKeyframe))
	{
		const FScopedTransaction Transaction(
			LOCTEXT("InsertKeyFrameBeforeTransaction", "Insert Key Frame Before"));
		GetFlipbookBeingEdited()->Modify();

		FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());

		FPaperFlipbookKeyFrame NewFrame;
		EditLock.KeyFrames.Insert(NewFrame, CurrentSelectedKeyframe);

		CurrentSelectedKeyframe = INDEX_NONE;
	}
}

void FC2DFrameInstructionsEditor::AddNewKeyFrameAfter()
{
	UPaperFlipbook* FlipbookBeingEdited = C2DFrameInstructionsBeingEdited->TargetFlipbook;

	if (FlipbookBeingEdited->IsValidKeyFrameIndex(CurrentSelectedKeyframe))
	{
		const FScopedTransaction Transaction(
			LOCTEXT("InsertKeyFrameAfterTransaction", "Insert Key Frame After"));
		FlipbookBeingEdited->Modify();

		FScopedFlipbookMutator EditLock(FlipbookBeingEdited);

		FPaperFlipbookKeyFrame NewFrame;
		EditLock.KeyFrames.Insert(NewFrame, CurrentSelectedKeyframe + 1);

		CurrentSelectedKeyframe = INDEX_NONE;
	}
}


void FC2DFrameInstructionsEditor::SetSelection(int32 NewSelection)
{
	CurrentSelectedKeyframe = NewSelection;
}

bool FC2DFrameInstructionsEditor::HasValidSelection() const
{
	return GetFlipbookBeingEdited()->
		IsValidKeyFrameIndex(CurrentSelectedKeyframe);
}

FReply FC2DFrameInstructionsEditor::OnClick_Forward()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();

	const bool bIsReverse = PreviewComponent->IsReversing();
	const bool bIsPlaying = PreviewComponent->IsPlaying();

	if (bIsReverse && bIsPlaying)
	{
		// Play forwards instead of backwards
		PreviewComponent->Play();
	}
	else if (bIsPlaying)
	{
		// Was already playing forwards, so pause
		PreviewComponent->Stop();
	}
	else
	{
		// Was paused, start playing
		PreviewComponent->Play();
	}

	return FReply::Handled();
}

FReply FC2DFrameInstructionsEditor::OnClick_Forward_Step()
{
	GetPreviewComponent()->Stop();
	SetCurrentFrame(GetCurrentFrame() + 1);
	return FReply::Handled();
}

FReply FC2DFrameInstructionsEditor::OnClick_Forward_End()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	PreviewComponent->Stop();
	PreviewComponent->SetPlaybackPosition(PreviewComponent->GetFlipbookLength(),
	                                      /*bFireEvents=*/ false);
	return FReply::Handled();
}

FReply FC2DFrameInstructionsEditor::OnClick_Backward()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();

	const bool bIsReverse = PreviewComponent->IsReversing();
	const bool bIsPlaying = PreviewComponent->IsPlaying();

	if (bIsReverse && bIsPlaying)
	{
		// Was already playing backwards, so pause
		PreviewComponent->Stop();
	}
	else if (bIsPlaying)
	{
		// Play backwards instead of forwards
		PreviewComponent->Reverse();
	}
	else
	{
		// Was paused, start reversing
		PreviewComponent->Reverse();
	}

	return FReply::Handled();
}

FReply FC2DFrameInstructionsEditor::OnClick_Backward_Step()
{
	GetPreviewComponent()->Stop();
	SetCurrentFrame(GetCurrentFrame() - 1);
	return FReply::Handled();
}

FReply FC2DFrameInstructionsEditor::OnClick_Backward_End()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	PreviewComponent->Stop();
	PreviewComponent->SetPlaybackPosition(0.0f, /*bFireEvents=*/ false);
	return FReply::Handled();
}

FReply FC2DFrameInstructionsEditor::OnClick_ToggleLoop()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	PreviewComponent->SetLooping(!PreviewComponent->IsLooping());
	return FReply::Handled();
}

EPlaybackMode::Type FC2DFrameInstructionsEditor::GetPlaybackMode() const
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	if (PreviewComponent->IsPlaying())
	{
		return PreviewComponent->IsReversing()
			       ? EPlaybackMode::PlayingReverse
			       : EPlaybackMode::PlayingForward;
	}
	else
	{
		return EPlaybackMode::Stopped;
	}
}

uint32 FC2DFrameInstructionsEditor::GetTotalFrameCount() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0 : GetFlipbookBeingEdited()->GetNumFrames();
}

uint32 FC2DFrameInstructionsEditor::GetTotalFrameCountPlusOne() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0 : GetFlipbookBeingEdited()->GetNumFrames() + 1;
}

float FC2DFrameInstructionsEditor::GetTotalSequenceLength() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0.f : GetFlipbookBeingEdited()->GetTotalDuration();
}

float FC2DFrameInstructionsEditor::GetPlaybackPosition() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0.f : GetPreviewComponent()->GetPlaybackPosition();
}

void FC2DFrameInstructionsEditor::SetPlaybackPosition(float NewTime)
{
	NewTime = FMath::Clamp<float>(NewTime, 0.0f, GetTotalSequenceLength());

	GetPreviewComponent()->SetPlaybackPosition(NewTime, /*bFireEvents=*/ false);
}

bool FC2DFrameInstructionsEditor::IsLooping() const
{
	return GetPreviewComponent()->IsLooping();
}

float FC2DFrameInstructionsEditor::GetViewRangeMin() const
{
	return ViewInputMin;
}

float FC2DFrameInstructionsEditor::GetViewRangeMax() const
{
	// See if the flipbook changed length, and if so reframe the scrub bar to include the full length
	//@TODO: This is a pretty odd place to put it, but there's no callback for a modified timeline at the moment, so...
	const float SequenceLength = GetTotalSequenceLength();
	if (SequenceLength != LastObservedSequenceLength)
	{
		LastObservedSequenceLength = SequenceLength;
		ViewInputMin = 0.0f;
		ViewInputMax = SequenceLength;
	}

	return ViewInputMax;
}

void FC2DFrameInstructionsEditor::SetViewRange(float NewMin, float NewMax)
{
	ViewInputMin = FMath::Max<float>(NewMin, 0.0f);
	ViewInputMax = FMath::Min<float>(NewMax, GetTotalSequenceLength());
}

float FC2DFrameInstructionsEditor::GetFramesPerSecond() const
{
	return C2DFrameInstructionsBeingEdited->TargetFlipbook->GetFramesPerSecond();
}

int32 FC2DFrameInstructionsEditor::GetCurrentFrame() const
{
	if (GetFlipbookBeingEdited() == nullptr)
	{
		return INDEX_NONE;
	}

	const int32 TotalLengthInFrames = GetTotalFrameCount();

	if (TotalLengthInFrames == 0)
	{
		return INDEX_NONE;
	}
	else
	{
		return FMath::Clamp<int32>((int32)(GetPlaybackPosition() * GetFramesPerSecond()), 0, TotalLengthInFrames);
	}
}

void FC2DFrameInstructionsEditor::SetCurrentFrame(int32 NewIndex)
{
	const int32 TotalLengthInFrames = GetTotalFrameCount();
	if (TotalLengthInFrames > 0)
	{
		int32 ClampedIndex = FMath::Clamp<int32>(NewIndex, 0, TotalLengthInFrames);
		SetPlaybackPosition(ClampedIndex / GetFramesPerSecond());
	}
	else
	{
		SetPlaybackPosition(0.0f);
	}
}

void FC2DFrameInstructionsEditor::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook,
                                                   const TSharedPtr<FUICommandList>& CommandList,
                                                   EExtensionHook::Position position)
{
	MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FC2DFrameInstructionsEditor::MakePulldownMenu(FMenuBarBuilder& menuBuilder)
{
	menuBuilder.AddPullDownMenu(
		FText::FromString("Example"),
		FText::FromString("Open the Example menu"),
		FNewMenuDelegate::CreateRaw(this, &FC2DFrameInstructionsEditor::FillPulldownMenu),
		"Example",
		FName(TEXT("ExampleMenu"))
	);
}

void FC2DFrameInstructionsEditor::FillPulldownMenu(FMenuBuilder& menuBuilder)
{
	
	// just a frame for tools to fill in
	menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 1"));
	menuBuilder.AddMenuSeparator(FName("Section_1"));
	menuBuilder.EndSection();

	menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 2"));
	menuBuilder.AddMenuSeparator(FName("Section_2"));
	menuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
