#include "MovesetEditor.h"

#include "CanvasItem.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"

#include "Combat2DModule.h"

#include "SScrubControlPanel.h"
#include "SCommonEditorViewportToolbarBase.h"

#include "SSingleObjectDetailsPanel.h"

#define LOCTEXT_NAMESPACE "MovesetEditor"

//////////////////////////////////////////////////////////////////////////

const FName MovesetEditorAppName = FName(TEXT("MovesetEditorApp"));

TSharedRef<FWorkspaceItem> FMovesetEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

struct FMovesetEditorTabs {
  // Tab identifiers
  static const FName DetailsID;
};

const FName FMovesetEditorTabs::DetailsID(TEXT("Details"));

/////////////////////////////////////////////////////
// SFlipbookPropertiesTabBody

class SMovesetPropertiesTabBody : public SSingleObjectDetailsPanel {
public:
SLATE_BEGIN_ARGS(SMovesetPropertiesTabBody) {
    }

  SLATE_END_ARGS()

private:
  // Pointer back to owning sprite editor instance (the keeper of state)
  TWeakPtr<class FMovesetEditor> MovesetEditorPtr;
public:
  void Construct(const FArguments &InArgs,
                 TSharedPtr<FMovesetEditor> InFlipbookEditor) {
    MovesetEditorPtr = InFlipbookEditor;

    SSingleObjectDetailsPanel::Construct(
        SSingleObjectDetailsPanel::FArguments().
        HostCommandList(InFlipbookEditor->GetToolkitCommands()).
        HostTabManager(InFlipbookEditor->GetTabManager()),
        /*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/
        true);
  }

  // SSingleObjectDetailsPanel interface
  virtual UObject *GetObjectToObserve() const override {
    return MovesetEditorPtr.Pin()->GetMovesetBeingEdited();
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

void FMovesetEditor::RegisterTabSpawners(
    const TSharedRef<class FTabManager> &InTabManager) {
  WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
      LOCTEXT("WorkspaceMenu_MovesetEditor", "Flipbook Data Editor"));

  FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

  InTabManager->RegisterTabSpawner(FMovesetEditorTabs::DetailsID,
                                   FOnSpawnTab::CreateSP(
                                       this,
                                       &FMovesetEditor::SpawnTab_Details))
              .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
              .SetGroup(WorkspaceMenuCategory.ToSharedRef())
              .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
                                  "LevelEditor.Tabs.Details"));
}

void FMovesetEditor::UnregisterTabSpawners(
    const TSharedRef<class FTabManager> &InTabManager) {
  FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

  InTabManager->UnregisterTabSpawner(FMovesetEditorTabs::DetailsID);
}

void FMovesetEditor::InitMovesetEditor(
    const EToolkitMode::Type Mode,
    const TSharedPtr<class IToolkitHost> &InitToolkitHost,
    UMoveset *InMoveset)
{
  MovesetBeingEdited = InMoveset;

  // FMovesetEditorCommands::Register();
	
  TSharedPtr<FMovesetEditor> MovesetEditorPtr = SharedThis(this);

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
              ->Split(
                  FTabManager::NewStack()
                  ->SetSizeCoefficient(0.25f)
                  ->AddTab(FMovesetEditorTabs::DetailsID,
                           ETabState::OpenedTab)
                  )
              ->Split(
                      FTabManager::NewStack()
                      ->SetSizeCoefficient(0.25f)
                      ->AddTab(FMovesetEditorTabs::DetailsID,
                          ETabState::OpenedTab)
                  )
              )
          );

  const bool bCreateDefaultStandaloneMenu = true;
  const bool bCreateDefaultToolbar = true;

  FAssetEditorToolkit::InitAssetEditor(
      Mode,
      InitToolkitHost,
      MovesetEditorAppIdentifier,
      StandaloneDefaultLayout,
      bCreateDefaultStandaloneMenu,
      bCreateDefaultToolbar,
      reinterpret_cast<UObject *>(InMoveset));

  ExtendToolbar();
  RegenerateMenusAndToolbars();
}

FMovesetEditor::~FMovesetEditor() {
  DetailsView.Reset();
  PropertiesTab.Reset();
}

FName FMovesetEditor::GetToolkitFName() const {
  return MovesetEditorAppName;
}

FText FMovesetEditor::GetBaseToolkitName() const {
  return LOCTEXT("AppLabel", "Flipbook Data Editor");
}

FText FMovesetEditor::GetToolkitToolTipText() const {
  return LOCTEXT("ToolTip", "Flipbook Data Editor");
}

FString FMovesetEditor::GetWorldCentricTabPrefix() const {
  return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString();
}

FLinearColor FMovesetEditor::GetWorldCentricTabColorScale() const {
  return FColor::Red;
}

UMoveset *FMovesetEditor::GetMovesetBeingEdited() {
  return MovesetBeingEdited;
}

void FMovesetEditor::ExtendToolbar() {

}

TSharedRef<SDockTab> FMovesetEditor::SpawnTab_Details(
    const FSpawnTabArgs &Args)
{
  TSharedPtr<FMovesetEditor> FlipbookEditorPtr = SharedThis(this);

  // Spawn the tab
  return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
         [
           SNew(SMovesetPropertiesTabBody, FlipbookEditorPtr)
         ];
}

void FMovesetEditor::CreateModeToolbarWidgets(
    FToolBarBuilder &IgnoredBuilder)
{
 
}

TSharedRef<SDockTab> FMovesetEditor::SpawnPropertiesTab(
    const FSpawnTabArgs &Args)
{
  check(Args.GetTabId() == FMovesetEditorTabs::DetailsID);

  return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
         [
           DetailsView.ToSharedRef()
         ];
}

void FMovesetEditor::AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList, EExtensionHook::Position position)
{
    MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FMovesetEditor::MakePulldownMenu(FMenuBarBuilder &menuBuilder)
{
    menuBuilder.AddPullDownMenu(
        FText::FromString("Example"),
        FText::FromString("Open the Example menu"),
        FNewMenuDelegate::CreateRaw(this, &FMovesetEditor::FillPulldownMenu),
        "Example",
        FName(TEXT("ExampleMenu"))
    );
}

void FMovesetEditor::FillPulldownMenu(FMenuBuilder &menuBuilder)
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
