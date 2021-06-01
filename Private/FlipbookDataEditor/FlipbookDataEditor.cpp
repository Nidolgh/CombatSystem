#include "FlipbookDataEditor.h"

#include "CanvasItem.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"

#include "Combat2DModule.h"

#include "Combat2DEditor/Classes/FlipbookData.h"
#include "FlipbookDataEditorCommands.h"
#include "FlipbookDataEditorViewportClient.h"
#include "SpriteGeometryEditCommands.h"
#include "SFlipbookDataEditorViewportToolbar.h"
#include "SScrubControlPanel.h"
#include "SFlipbookDataTimeline.h"
#include "SCommonEditorViewportToolbarBase.h"

#include "SEditorViewport.h"
#include "SSingleObjectDetailsPanel.h"

#define LOCTEXT_NAMESPACE "FlipbookDataEditor"

//////////////////////////////////////////////////////////////////////////

const FName FlipbookDataEditorAppName = FName(TEXT("FlipbookDataEditorApp"));

TSharedRef<FWorkspaceItem> FFlipbookDataEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

struct FFlipbookDataEditorTabs {
  // Tab identifiers
  static const FName DetailsID;
  static const FName ViewportID;
  static const FName GeometryID;
};

const FName FFlipbookDataEditorTabs::DetailsID(TEXT("Details"));
const FName FFlipbookDataEditorTabs::ViewportID(TEXT("Viewport"));
const FName FFlipbookDataEditorTabs::GeometryID(TEXT("Geometry"));

/////////////////////////////////////////////////////
// SFlipbookPropertiesTabBody

class SFlipbookDataPropertiesTabBody : public SSingleObjectDetailsPanel {
public:
SLATE_BEGIN_ARGS(SFlipbookDataPropertiesTabBody) {
    }

  SLATE_END_ARGS()

private:
  // Pointer back to owning sprite editor instance (the keeper of state)
  TWeakPtr<class FFlipbookDataEditor> FlipbookDataEditorPtr;
public:
  void Construct(const FArguments &InArgs,
                 TSharedPtr<FFlipbookDataEditor> InFlipbookEditor) {
    FlipbookDataEditorPtr = InFlipbookEditor;

    SSingleObjectDetailsPanel::Construct(
        SSingleObjectDetailsPanel::FArguments().
        HostCommandList(InFlipbookEditor->GetToolkitCommands()).
        HostTabManager(InFlipbookEditor->GetTabManager()),
        /*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/
        true);
  }

  // SSingleObjectDetailsPanel interface
  virtual UObject *GetObjectToObserve() const override {
    return FlipbookDataEditorPtr.Pin()->GetFlipbookDataBeingEdited();
  }

  virtual TSharedRef<SWidget> PopulateSlot(
      TSharedRef<SWidget> PropertyEditorWidget) override {
    
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
// SSpriteEditorViewport

class SFlipbookDataEditorViewport
    : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider {
public:
SLATE_BEGIN_ARGS(SFlipbookDataEditorViewport) {
    }

  SLATE_END_ARGS()

  void Construct(const FArguments &InArgs,
                 TSharedPtr<FFlipbookDataEditor> InFlipbookDataEditor);

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

  UPaperFlipbookComponent *GetPreviewComponent() const;

private:
  // Pointer back to owning flipbookData editor instance (the keeper of state)
  TWeakPtr<class FFlipbookDataEditor> FlipbookDataEditorPtr;

  // Viewport client
  TSharedPtr<FFlipbookDataEditorViewportClient> EditorViewportClient;
};

void SFlipbookDataEditorViewport::Construct(const FArguments &InArgs,
                                            TSharedPtr<FFlipbookDataEditor>
                                            InFlipbookDataEditor) {
  FlipbookDataEditorPtr = InFlipbookDataEditor;

  SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SFlipbookDataEditorViewport::BindCommands() {
  SEditorViewport::BindCommands();

  const FFlipbookDataEditorCommands &Commands =
      FFlipbookDataEditorCommands::Get();

  TSharedRef<FFlipbookDataEditorViewportClient> EditorViewportClientRef = EditorViewportClient.ToSharedRef();
	

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
                               &FFlipbookDataEditorViewportClient::ToggleShowPivot),
      FCanExecuteAction(),
      FIsActionChecked::CreateSP(EditorViewportClientRef,
                                 &FFlipbookDataEditorViewportClient::IsShowPivotChecked));


  const FFlipbookDataGeometryEditCommands& Commands2 = FFlipbookDataGeometryEditCommands::Get();
  const TSharedRef<FSpriteGeometryEditMode> GeometryEditRef = EditorViewportClientRef.Get().GeometryEditMode.ToSharedRef();
  FSpriteGeometryEditingHelper* GeometryHelper = GeometryEditRef->GetGeometryHelper();

  CommandList->MapAction(
      Commands2.AddBoxShape,
      FExecuteAction::CreateSP(EditorViewportClientRef.Get().GeometryEditMode.ToSharedRef(), &FSpriteGeometryEditMode::AddBoxShape),
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
SFlipbookDataEditorViewport::MakeEditorViewportClient() {
  EditorViewportClient = MakeShareable(
      new FFlipbookDataEditorViewportClient(FlipbookDataEditorPtr,
                                            SharedThis(this)));

  return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SFlipbookDataEditorViewport::MakeViewportToolbar() {
  return SNew(SFlipbookDataEditorViewportToolbar, SharedThis(this));
}

EVisibility SFlipbookDataEditorViewport::GetTransformToolbarVisibility() const {
  return EVisibility::Visible;
}

void SFlipbookDataEditorViewport::OnFocusViewportToSelection() {
  //EditorViewportClient->RequestFocusOnSelection(/*bInstant=*/ false);
}

TSharedRef<class SEditorViewport>
SFlipbookDataEditorViewport::GetViewportWidget() {
  return SharedThis(this);
}

TSharedPtr<FExtender> SFlipbookDataEditorViewport::GetExtenders() const {
  TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
  return Result;
}

void SFlipbookDataEditorViewport::OnFloatingButtonClicked() {
}

void SFlipbookDataEditorViewport::ShowExtractSpritesDialog() {
  /*if (UPaperSprite* Sprite = SpriteEditorPtr.Pin()->GetSpriteBeingEdited())
  {
          if (UTexture2D* SourceTexture = Sprite->GetSourceTexture())
          {
                  SPaperExtractSpritesDialog::ShowWindow(SourceTexture);
          }
  }*/
}

UPaperFlipbookComponent *
SFlipbookDataEditorViewport::GetPreviewComponent() const {
  return EditorViewportClient->GetPreviewComponent();
}

//////////////////////////////////////////////////////////////////////////

void FFlipbookDataEditor::RegisterTabSpawners(
    const TSharedRef<class FTabManager> &InTabManager) {
  WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
      LOCTEXT("WorkspaceMenu_FlipbookDataEditor", "Flipbook Data Editor"));

  FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

  InTabManager->RegisterTabSpawner(FFlipbookDataEditorTabs::ViewportID,
                                   FOnSpawnTab::CreateSP(
                                       this,
                                       &FFlipbookDataEditor::SpawnTab_Viewport))
              .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
              .SetGroup(WorkspaceMenuCategory.ToSharedRef())
              .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
                                  "LevelEditor.Tabs.Viewports"));

  InTabManager->RegisterTabSpawner(FFlipbookDataEditorTabs::DetailsID,
                                   FOnSpawnTab::CreateSP(
                                       this,
                                       &FFlipbookDataEditor::SpawnTab_Details))
              .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
              .SetGroup(WorkspaceMenuCategory.ToSharedRef())
              .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
                                  "LevelEditor.Tabs.Details"));

  InTabManager->RegisterTabSpawner(FFlipbookDataEditorTabs::GeometryID,
      FOnSpawnTab::CreateSP(
          this,
          &FFlipbookDataEditor::SpawnTab_Geometry))
      .SetDisplayName(LOCTEXT("GeometryTab", "Geometry"))
      .SetGroup(WorkspaceMenuCategory.ToSharedRef())
      .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
          "LevelEditor.Tabs.Geometry"));
}

void FFlipbookDataEditor::UnregisterTabSpawners(
    const TSharedRef<class FTabManager> &InTabManager) {
  FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

  InTabManager->UnregisterTabSpawner(FFlipbookDataEditorTabs::DetailsID);
}

void FFlipbookDataEditor::InitFlipbookDataEditor(
    const EToolkitMode::Type Mode,
    const TSharedPtr<class IToolkitHost> &InitToolkitHost,
    UFlipbookData *InFlipbookData) {
  FlipbookDataBeingEdited = InFlipbookData;

  FFlipbookDataEditorCommands::Register();
  FFlipbookDataGeometryEditCommands::Register();

  TSharedPtr<FFlipbookDataEditor> FlipbookDataEditorPtr = SharedThis(this);
  ViewportPtr = SNew(SFlipbookDataEditorViewport, FlipbookDataEditorPtr);

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
                  ->AddTab(FFlipbookDataEditorTabs::ViewportID,
                           ETabState::OpenedTab)
                  )
              ->Split(
                  FTabManager::NewStack()
                  ->SetSizeCoefficient(0.25f)
                  ->AddTab(FFlipbookDataEditorTabs::DetailsID,
                           ETabState::OpenedTab)
                  )
              ->Split(
                      FTabManager::NewStack()
                      ->SetSizeCoefficient(0.25f)
                      ->AddTab(FFlipbookDataEditorTabs::DetailsID,
                          ETabState::OpenedTab)
                  )
              )
          );

  const bool bCreateDefaultStandaloneMenu = true;
  const bool bCreateDefaultToolbar = true;

  FAssetEditorToolkit::InitAssetEditor(
      Mode,
      InitToolkitHost,
      FlipbookDataEditorAppIdentifier,
      StandaloneDefaultLayout,
      bCreateDefaultStandaloneMenu,
      bCreateDefaultToolbar,
      reinterpret_cast<UObject *>(InFlipbookData));

  ExtendToolbar();
  RegenerateMenusAndToolbars();
}

FFlipbookDataEditor::~FFlipbookDataEditor() {
  DetailsView.Reset();
  PropertiesTab.Reset();
}

UPaperFlipbookComponent *FFlipbookDataEditor::GetPreviewComponent() const {
  UPaperFlipbookComponent *PreviewComponent = ViewportPtr->
      GetPreviewComponent();
  check(PreviewComponent);
  return PreviewComponent;
}

FName FFlipbookDataEditor::GetToolkitFName() const {
  return FlipbookDataEditorAppName;
}

FText FFlipbookDataEditor::GetBaseToolkitName() const {
  return LOCTEXT("AppLabel", "Flipbook Data Editor");
}

FText FFlipbookDataEditor::GetToolkitToolTipText() const {
  return LOCTEXT("ToolTip", "Flipbook Data Editor");
}

FString FFlipbookDataEditor::GetWorldCentricTabPrefix() const {
  return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString();
}

FLinearColor FFlipbookDataEditor::GetWorldCentricTabColorScale() const {
  return FColor::Red;
}

UFlipbookData *FFlipbookDataEditor::GetFlipbookDataBeingEdited() {
  return FlipbookDataBeingEdited;
}

UPaperFlipbook *FFlipbookDataEditor::GetFlipbookBeingEdited() const {
  return FlipbookDataBeingEdited->TargetFlipbook;
}

FFlipbookDataKeyFrame* FFlipbookDataEditor::CreateKeyFrameDataOnCurrentFrame()
{
	const int32 curFrame = GetCurrentFrame();
    
	TArray<FFlipbookDataKeyFrame>& keyFrameArray = GetFlipbookDataBeingEdited()->KeyFrameArray;

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

void FFlipbookDataEditor::CreateKeyFrameInstructionOnCurrentFrame()
{
    const int32 curFrame = GetCurrentFrame();
	
    if (curFrame == INDEX_NONE)
    {
        return;
    }
	
    TArray<FFlipbookDataKeyFrame>& keyFrameArray = GetFlipbookDataBeingEdited()->KeyFrameArray;

	if (keyFrameArray.IsValidIndex(curFrame))
	{
        keyFrameArray[curFrame].KeyFrameInstructions.AddDefaulted();
    }
}

void FFlipbookDataEditor::ExtendToolbar() {
  struct Local {
    static void FillToolbar(FToolBarBuilder &ToolbarBuilder) {
      const FFlipbookDataGeometryEditCommands &GeometryCommands =
          FFlipbookDataGeometryEditCommands::Get();
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
      MenuExtender->AddMenuBarExtension("Flipbook Data Editor", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FFlipbookDataEditor::MakePulldownMenu));
      LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
  }*/

  //ICombat2DModule* Combat2DModule = &FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
  //AddToolbarExtender(Combat2DModule->GetSpriteEditorToolBarExtensibilityManager()->GetAllExtenders());
}

TSharedRef<SDockTab> FFlipbookDataEditor::SpawnTab_Viewport(
    const FSpawnTabArgs &Args) {
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
                                         &FFlipbookDataEditor::GetPlaybackPosition)
                                     .NumOfKeys(
                                         this,
                                         &FFlipbookDataEditor::GetTotalFrameCountPlusOne)
                                     .SequenceLength(
                                         this,
                                         &FFlipbookDataEditor::GetTotalSequenceLength)
                                     .OnValueChanged(
                                         this,
                                         &FFlipbookDataEditor::SetPlaybackPosition)
                                     //		.OnBeginSliderMovement(this, &SAnimationScrubPanel::OnBeginSliderMovement)
                                     //		.OnEndSliderMovement(this, &SAnimationScrubPanel::OnEndSliderMovement)
                                     .OnClickedForwardPlay(
                                         this,
                                         &FFlipbookDataEditor::OnClick_Forward)
                                     .OnClickedForwardStep(
                                         this,
                                         &FFlipbookDataEditor::OnClick_Forward_Step)
                                     .OnClickedForwardEnd(
                                         this,
                                         &FFlipbookDataEditor::OnClick_Forward_End)
                                     .OnClickedBackwardPlay(
                                         this,
                                         &FFlipbookDataEditor::OnClick_Backward)
                                     .OnClickedBackwardStep(
                                         this,
                                         &FFlipbookDataEditor::OnClick_Backward_Step)
                                     .OnClickedBackwardEnd(
                                         this,
                                         &FFlipbookDataEditor::OnClick_Backward_End)
                                     .OnClickedToggleLoop(
                                         this,
                                         &FFlipbookDataEditor::OnClick_ToggleLoop)
                                     .OnGetLooping(
                                         this, &FFlipbookDataEditor::IsLooping)
                                     .OnGetPlaybackMode(
                                         this,
                                         &FFlipbookDataEditor::GetPlaybackMode)
                                     .ViewInputMin(
                                         this,
                                         &FFlipbookDataEditor::GetViewRangeMin)
                                     .ViewInputMax(
                                         this,
                                         &FFlipbookDataEditor::GetViewRangeMax)
                                     .OnSetInputViewRange(
                                         this,
                                         &FFlipbookDataEditor::SetViewRange)
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
                 SNew(SFlipbookDataTimeline, GetToolkitCommands())
                 .FlipbookDataBeingEdited(GetFlipbookDataBeingEdited())
                 .OnSelectionChanged(this, &FFlipbookDataEditor::SetSelection)
                 .PlayTime(this, &FFlipbookDataEditor::GetPlaybackPosition)
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

TSharedRef<SDockTab> FFlipbookDataEditor::SpawnTab_Details(
    const FSpawnTabArgs &Args)
{
  TSharedPtr<FFlipbookDataEditor> FlipbookEditorPtr = SharedThis(this);

  // Spawn the tab
  return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
         [
           SNew(SFlipbookDataPropertiesTabBody, FlipbookEditorPtr)
         ];
}

TSharedRef<SDockTab> FFlipbookDataEditor::SpawnTab_Geometry(
    const FSpawnTabArgs &Args)
{
    TSharedPtr<FFlipbookDataEditor> FlipbookEditorPtr = SharedThis(this);

    GeometryPropertiesTabBody = SNew(SGeometryPropertiesTabBody, FlipbookEditorPtr);

    // Spawn the tab
    return SNew(SDockTab)
        .Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Geometry"))
        .Label(LOCTEXT("GeometryTab_Title", "Geometry"))
        [
            GeometryPropertiesTabBody.ToSharedRef()
        ];
}

void FFlipbookDataEditor::CreateModeToolbarWidgets(
    FToolBarBuilder &IgnoredBuilder) {
  FToolBarBuilder ToolbarBuilder(ViewportPtr->GetCommandList(),
                                 FMultiBoxCustomization::None);
  ToolbarBuilder.AddToolBarButton(
      FFlipbookDataEditorCommands::Get().EnterViewMode);
  ToolbarBuilder.AddToolBarButton(
      FFlipbookDataEditorCommands::Get().EnterSourceRegionEditMode);
  ToolbarBuilder.AddToolBarButton(
      FFlipbookDataEditorCommands::Get().EnterCollisionEditMode);
  ToolbarBuilder.AddToolBarButton(
      FFlipbookDataEditorCommands::Get().EnterRenderingEditMode);
  AddToolbarWidget(ToolbarBuilder.MakeWidget());
}

TSharedRef<SDockTab> FFlipbookDataEditor::SpawnPropertiesTab(
    const FSpawnTabArgs &Args) {
  check(Args.GetTabId() == FFlipbookDataEditorTabs::DetailsID);

  return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
         [
           DetailsView.ToSharedRef()
         ];
}

void FFlipbookDataEditor::AddKeyFrameAtCurrentTime() {
  const FScopedTransaction Transaction(
      LOCTEXT("InsertKeyFrameTransaction", "Insert Key Frame"));
  FlipbookDataBeingEdited->Modify();

  const float CurrentTime = GetPlaybackPosition();
  const int32 KeyFrameIndex = GetFlipbookBeingEdited()->GetKeyFrameIndexAtTime(
      CurrentTime);
  const int32 ClampedIndex = FMath::Clamp<int32>(
      KeyFrameIndex, 0, GetFlipbookBeingEdited()->GetNumFrames());

  FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());
  FPaperFlipbookKeyFrame NewFrame;
  EditLock.KeyFrames.Insert(NewFrame, ClampedIndex);
}

void FFlipbookDataEditor::AddNewKeyFrameAtEnd() {
  const FScopedTransaction Transaction(
      LOCTEXT("AddKeyFrameTransaction", "Add Key Frame"));
  GetFlipbookBeingEdited()->Modify();

  FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());

  FPaperFlipbookKeyFrame &NewFrame = *new(EditLock.KeyFrames)
      FPaperFlipbookKeyFrame();
}

void FFlipbookDataEditor::AddNewKeyFrameBefore() {
  if (GetFlipbookBeingEdited()->IsValidKeyFrameIndex(CurrentSelectedKeyframe)) {
    const FScopedTransaction Transaction(
        LOCTEXT("InsertKeyFrameBeforeTransaction", "Insert Key Frame Before"));
    GetFlipbookBeingEdited()->Modify();

    FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());

    FPaperFlipbookKeyFrame NewFrame;
    EditLock.KeyFrames.Insert(NewFrame, CurrentSelectedKeyframe);

    CurrentSelectedKeyframe = INDEX_NONE;
  }
}

void FFlipbookDataEditor::AddNewKeyFrameAfter() {
  UPaperFlipbook *FlipbookBeingEdited = FlipbookDataBeingEdited->TargetFlipbook;

  if (FlipbookBeingEdited->IsValidKeyFrameIndex(CurrentSelectedKeyframe)) {
    const FScopedTransaction Transaction(
        LOCTEXT("InsertKeyFrameAfterTransaction", "Insert Key Frame After"));
    FlipbookBeingEdited->Modify();

    FScopedFlipbookMutator EditLock(FlipbookBeingEdited);

    FPaperFlipbookKeyFrame NewFrame;
    EditLock.KeyFrames.Insert(NewFrame, CurrentSelectedKeyframe + 1);

    CurrentSelectedKeyframe = INDEX_NONE;
  }
}


void FFlipbookDataEditor::SetSelection(int32 NewSelection) {
  CurrentSelectedKeyframe = NewSelection;
}

bool FFlipbookDataEditor::HasValidSelection() const {
  return GetFlipbookBeingEdited()->
      IsValidKeyFrameIndex(CurrentSelectedKeyframe);
}

FReply FFlipbookDataEditor::OnClick_Forward() {
  UPaperFlipbookComponent *PreviewComponent = GetPreviewComponent();

  const bool bIsReverse = PreviewComponent->IsReversing();
  const bool bIsPlaying = PreviewComponent->IsPlaying();

  if (bIsReverse && bIsPlaying) {
    // Play forwards instead of backwards
    PreviewComponent->Play();
  } else if (bIsPlaying) {
    // Was already playing forwards, so pause
    PreviewComponent->Stop();
  } else {
    // Was paused, start playing
    PreviewComponent->Play();
  }

  return FReply::Handled();
}

FReply FFlipbookDataEditor::OnClick_Forward_Step() {
  GetPreviewComponent()->Stop();
  SetCurrentFrame(GetCurrentFrame() + 1);
  return FReply::Handled();
}

FReply FFlipbookDataEditor::OnClick_Forward_End() {
  UPaperFlipbookComponent *PreviewComponent = GetPreviewComponent();
  PreviewComponent->Stop();
  PreviewComponent->SetPlaybackPosition(PreviewComponent->GetFlipbookLength(),
                                        /*bFireEvents=*/ false);
  return FReply::Handled();
}

FReply FFlipbookDataEditor::OnClick_Backward() {
  UPaperFlipbookComponent *PreviewComponent = GetPreviewComponent();

  const bool bIsReverse = PreviewComponent->IsReversing();
  const bool bIsPlaying = PreviewComponent->IsPlaying();

  if (bIsReverse && bIsPlaying) {
    // Was already playing backwards, so pause
    PreviewComponent->Stop();
  } else if (bIsPlaying) {
    // Play backwards instead of forwards
    PreviewComponent->Reverse();
  } else {
    // Was paused, start reversing
    PreviewComponent->Reverse();
  }

  return FReply::Handled();
}

FReply FFlipbookDataEditor::OnClick_Backward_Step() {
  GetPreviewComponent()->Stop();
  SetCurrentFrame(GetCurrentFrame() - 1);
  return FReply::Handled();
}

FReply FFlipbookDataEditor::OnClick_Backward_End() {
  UPaperFlipbookComponent *PreviewComponent = GetPreviewComponent();
  PreviewComponent->Stop();
  PreviewComponent->SetPlaybackPosition(0.0f, /*bFireEvents=*/ false);
  return FReply::Handled();
}

FReply FFlipbookDataEditor::OnClick_ToggleLoop() {
  UPaperFlipbookComponent *PreviewComponent = GetPreviewComponent();
  PreviewComponent->SetLooping(!PreviewComponent->IsLooping());
  return FReply::Handled();
}

EPlaybackMode::Type FFlipbookDataEditor::GetPlaybackMode() const {
  UPaperFlipbookComponent *PreviewComponent = GetPreviewComponent();
  if (PreviewComponent->IsPlaying()) {
    return PreviewComponent->IsReversing()
             ? EPlaybackMode::PlayingReverse
             : EPlaybackMode::PlayingForward;
  } else {
    return EPlaybackMode::Stopped;
  }
}

uint32 FFlipbookDataEditor::GetTotalFrameCount() const {
  return GetFlipbookBeingEdited() == nullptr ? 0 : GetFlipbookBeingEdited()->GetNumFrames();
}

uint32 FFlipbookDataEditor::GetTotalFrameCountPlusOne() const {
  return GetFlipbookBeingEdited() == nullptr ? 0 : GetFlipbookBeingEdited()->GetNumFrames() + 1;
}

float FFlipbookDataEditor::GetTotalSequenceLength() const {
  return GetFlipbookBeingEdited() == nullptr ? 0.f : GetFlipbookBeingEdited()->GetTotalDuration();
}

float FFlipbookDataEditor::GetPlaybackPosition() const {
  return GetFlipbookBeingEdited() == nullptr ? 0.f : GetPreviewComponent()->GetPlaybackPosition();
}

void FFlipbookDataEditor::SetPlaybackPosition(float NewTime) {
  NewTime = FMath::Clamp<float>(NewTime, 0.0f, GetTotalSequenceLength());

  GetPreviewComponent()->SetPlaybackPosition(NewTime, /*bFireEvents=*/ false);
}

bool FFlipbookDataEditor::IsLooping() const {
  return GetPreviewComponent()->IsLooping();
}

float FFlipbookDataEditor::GetViewRangeMin() const {
  return ViewInputMin;
}

float FFlipbookDataEditor::GetViewRangeMax() const {
  // See if the flipbook changed length, and if so reframe the scrub bar to include the full length
  //@TODO: This is a pretty odd place to put it, but there's no callback for a modified timeline at the moment, so...
  const float SequenceLength = GetTotalSequenceLength();
  if (SequenceLength != LastObservedSequenceLength) {
    LastObservedSequenceLength = SequenceLength;
    ViewInputMin = 0.0f;
    ViewInputMax = SequenceLength;
  }

  return ViewInputMax;
}

void FFlipbookDataEditor::SetViewRange(float NewMin, float NewMax) {
  ViewInputMin = FMath::Max<float>(NewMin, 0.0f);
  ViewInputMax = FMath::Min<float>(NewMax, GetTotalSequenceLength());
}

float FFlipbookDataEditor::GetFramesPerSecond() const
{
    return FlipbookDataBeingEdited->TargetFlipbook->GetFramesPerSecond();
}

int32 FFlipbookDataEditor::GetCurrentFrame() const
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

void FFlipbookDataEditor::SetCurrentFrame(int32 NewIndex)
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

void FFlipbookDataEditor::AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList, EExtensionHook::Position position)
{
    MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FFlipbookDataEditor::MakePulldownMenu(FMenuBarBuilder &menuBuilder)
{
    menuBuilder.AddPullDownMenu(
        FText::FromString("Example"),
        FText::FromString("Open the Example menu"),
        FNewMenuDelegate::CreateRaw(this, &FFlipbookDataEditor::FillPulldownMenu),
        "Example",
        FName(TEXT("ExampleMenu"))
    );
}

void FFlipbookDataEditor::FillPulldownMenu(FMenuBuilder &menuBuilder)
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
