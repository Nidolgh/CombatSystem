#include "CombatMovesetEditor.h"

#include "CanvasItem.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"

#include "Combat2DEditorModule.h"

#include "SScrubControlPanel.h"
#include "SCommonEditorViewportToolbarBase.h"

#include "SSingleObjectDetailsPanel.h"

#define LOCTEXT_NAMESPACE "CombatMovesetEditor"

//////////////////////////////////////////////////////////////////////////

const FName MovesetEditorAppName = FName(TEXT("CombatMovesetEditorApp"));

TSharedRef<FWorkspaceItem> FCombatMovesetEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

struct FCombatMovesetEditorTabs {
    // Tab identifiers
    static const FName DetailsID;
};

const FName FCombatMovesetEditorTabs::DetailsID(TEXT("Details"));

/////////////////////////////////////////////////////
// SFlipbookPropertiesTabBody

class SMovesetPropertiesTabBody : public SSingleObjectDetailsPanel {
public:
    SLATE_BEGIN_ARGS(SMovesetPropertiesTabBody) {
    }

    SLATE_END_ARGS()

private:
    // Pointer back to owning sprite editor instance (the keeper of state)
    TWeakPtr<class FCombatMovesetEditor> CombatMovesetEditorPtr;
public:
    void Construct(const FArguments &InArgs,
        TSharedPtr<FCombatMovesetEditor> InFlipbookEditor) {
        CombatMovesetEditorPtr = InFlipbookEditor;

        SSingleObjectDetailsPanel::Construct(
            SSingleObjectDetailsPanel::FArguments().
            HostCommandList(InFlipbookEditor->GetToolkitCommands()).
            HostTabManager(InFlipbookEditor->GetTabManager()),
            /*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/
            true);
    }

    // SSingleObjectDetailsPanel interface
    virtual UObject *GetObjectToObserve() const override {
        return CombatMovesetEditorPtr.Pin()->GetCombatMovesetBeingEdited();
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

void FCombatMovesetEditor::RegisterTabSpawners(
    const TSharedRef<class FTabManager> &InTabManager) {
    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
        LOCTEXT("WorkspaceMenu_MovesetEditor", "Flipbook Data Editor"));

    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(FCombatMovesetEditorTabs::DetailsID,
        FOnSpawnTab::CreateSP(
            this,
            &FCombatMovesetEditor::SpawnTab_Details))
        .SetDisplayName(LOCTEXT("DetailsTab", "Details"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
            "LevelEditor.Tabs.Details"));
}

void FCombatMovesetEditor::UnregisterTabSpawners(
    const TSharedRef<class FTabManager> &InTabManager) {
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

    InTabManager->UnregisterTabSpawner(FCombatMovesetEditorTabs::DetailsID);
}

void FCombatMovesetEditor::InitCombatMovesetEditor(
    const EToolkitMode::Type Mode,
    const TSharedPtr<class IToolkitHost> &InitToolkitHost,
    UCombatMoveset *InMoveset)
{
    CombatMovesetBeingEdited = InMoveset;

    // FCombatMovesetEditorCommands::Register();

    TSharedPtr<FCombatMovesetEditor> MovesetEditorPtr = SharedThis(this);

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
                    ->AddTab(FCombatMovesetEditorTabs::DetailsID,
                        ETabState::OpenedTab)
                )
                ->Split(
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.25f)
                    ->AddTab(FCombatMovesetEditorTabs::DetailsID,
                        ETabState::OpenedTab)
                )
            )
        );

    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = true;

    FAssetEditorToolkit::InitAssetEditor(
        Mode,
        InitToolkitHost,
        CombatMovesetEditorAppIdentifier,
        StandaloneDefaultLayout,
        bCreateDefaultStandaloneMenu,
        bCreateDefaultToolbar,
        reinterpret_cast<UObject *>(InMoveset));

    ExtendToolbar();
    RegenerateMenusAndToolbars();
}

FCombatMovesetEditor::~FCombatMovesetEditor() {
    DetailsView.Reset();
    PropertiesTab.Reset();
}

FName FCombatMovesetEditor::GetToolkitFName() const {
    return MovesetEditorAppName;
}

FText FCombatMovesetEditor::GetBaseToolkitName() const {
    return LOCTEXT("AppLabel", "Flipbook Data Editor");
}

FText FCombatMovesetEditor::GetToolkitToolTipText() const {
    return LOCTEXT("ToolTip", "Flipbook Data Editor");
}

FString FCombatMovesetEditor::GetWorldCentricTabPrefix() const {
    return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString();
}

FLinearColor FCombatMovesetEditor::GetWorldCentricTabColorScale() const {
    return FColor::Red;
}

UCombatMoveset *FCombatMovesetEditor::GetCombatMovesetBeingEdited() {
    return CombatMovesetBeingEdited;
}

void FCombatMovesetEditor::ExtendToolbar() {

}

TSharedRef<SDockTab> FCombatMovesetEditor::SpawnTab_Details(
    const FSpawnTabArgs &Args)
{
    TSharedPtr<FCombatMovesetEditor> FlipbookEditorPtr = SharedThis(this);

    // Spawn the tab
    return SNew(SDockTab)
        .Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
        .Label(LOCTEXT("DetailsTab_Title", "Details"))
        [
            SNew(SMovesetPropertiesTabBody, FlipbookEditorPtr)
        ];
}

void FCombatMovesetEditor::CreateModeToolbarWidgets(
    FToolBarBuilder &IgnoredBuilder)
{

}

TSharedRef<SDockTab> FCombatMovesetEditor::SpawnPropertiesTab(
    const FSpawnTabArgs &Args)
{
    check(Args.GetTabId() == FCombatMovesetEditorTabs::DetailsID);

    return SNew(SDockTab)
        .Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
        .Label(LOCTEXT("GenericDetailsTitle", "Details"))
        .TabColorScale(GetTabColorScale())
        [
            DetailsView.ToSharedRef()
        ];
}

void FCombatMovesetEditor::AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList, EExtensionHook::Position position)
{
    MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FCombatMovesetEditor::MakePulldownMenu(FMenuBarBuilder &menuBuilder)
{
    menuBuilder.AddPullDownMenu(
        FText::FromString("Example"),
        FText::FromString("Open the Example menu"),
        FNewMenuDelegate::CreateRaw(this, &FCombatMovesetEditor::FillPulldownMenu),
        "Example",
        FName(TEXT("ExampleMenu"))
    );
}

void FCombatMovesetEditor::FillPulldownMenu(FMenuBuilder &menuBuilder)
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
