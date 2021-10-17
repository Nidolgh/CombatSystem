#include "CombatFlipbookEditor.h"

#include "Combat2DEditorModule.h"

#include "CombatFlipbook.h"
#include "CombatFlipbookEditorCommands.h"
#include "CombatFlipbookEditorViewport.h"
#include "EditorStyleSet.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "PaperFlipbookComponent.h"
#include "SpriteEditing/CombatSpriteGeometryEditCommands.h"

#include "SScrubControlPanel.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SCombatFlipbookDetailsTab.h"
#include "Widgets/SFrameCollisionDataTab.h"
#include "Widgets/STimeline/SCombatFlipbookTimeline.h"

#define LOCTEXT_NAMESPACE "CombatFlipbookEditor"

const FName CombatFlipbookEditorAppName = FName(TEXT("CombatFlipbookEditorApp"));
TSharedRef<FWorkspaceItem> FCombatFlipbookEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

struct FCombatFlipbookEditorTabs
{
	// Tab identifiers
	static const FName DetailsID;
	static const FName ViewportID;
	static const FName GeometryID;
};

const FName FCombatFlipbookEditorTabs::DetailsID(TEXT("Details"));
const FName FCombatFlipbookEditorTabs::ViewportID(TEXT("Viewport"));
const FName FCombatFlipbookEditorTabs::GeometryID(TEXT("Geometry"));

void FCombatFlipbookEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
		LOCTEXT("WorkspaceMenu_CombatFlipbookEditor", "Flipbook Data Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FCombatFlipbookEditorTabs::ViewportID,
		FOnSpawnTab::CreateSP(
			this,
			&FCombatFlipbookEditor::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FCombatFlipbookEditorTabs::DetailsID,
		FOnSpawnTab::CreateSP(
			this,
			&FCombatFlipbookEditor::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FCombatFlipbookEditorTabs::GeometryID,
		FOnSpawnTab::CreateSP(
			this,
			&FCombatFlipbookEditor::SpawnTab_Geometry))
		.SetDisplayName(LOCTEXT("GeometryTab", "Geometry"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.Tabs.Geometry"));
}

void FCombatFlipbookEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FCombatFlipbookEditorTabs::DetailsID);
}

void FCombatFlipbookEditor::InitCombatFlipbookEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UCombatFlipbook* InCombatFlipbook)
{
	CombatFlipbookBeingEdited = InCombatFlipbook;

	FCombatFlipbookEditorCommands::Register();
	FCombatSpriteGeometryEditCommands::Register();

	TSharedPtr<FCombatFlipbookEditor> CombatFlipbookEditorPtr = SharedThis(this);
	ViewportPtr = SNew(SCombatFlipbookEditorViewport, CombatFlipbookEditorPtr);

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
					->AddTab(FCombatFlipbookEditorTabs::ViewportID,
						ETabState::OpenedTab)
				)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(FCombatFlipbookEditorTabs::DetailsID,
						ETabState::OpenedTab)
				)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(FCombatFlipbookEditorTabs::DetailsID,
						ETabState::OpenedTab)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		CombatFlipbookEditorAppIdentifier,
		StandaloneDefaultLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		reinterpret_cast<UObject*>(InCombatFlipbook));

	ActivateEditMode();
	
	ExtendToolbar();
	RegenerateMenusAndToolbars();
}

void FCombatFlipbookEditor::ActivateEditMode()
{
	ViewportPtr->ActivateEditMode();
}

FCombatFlipbookEditor::~FCombatFlipbookEditor()
{
	DetailsView.Reset();
	PropertiesTab.Reset();
}

UPaperFlipbookComponent* FCombatFlipbookEditor::GetPreviewComponent() const
{
	UPaperFlipbookComponent* PreviewComponent = ViewportPtr->
		GetPreviewComponent();
	check(PreviewComponent);
	return PreviewComponent;
}

FName FCombatFlipbookEditor::GetToolkitFName() const
{
	return CombatFlipbookEditorAppName;
}

FText FCombatFlipbookEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Flipbook Data Editor");
}

FText FCombatFlipbookEditor::GetToolkitToolTipText() const
{
	return LOCTEXT("ToolTip", "Flipbook Data Editor");
}

FString FCombatFlipbookEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString();
}

FLinearColor FCombatFlipbookEditor::GetWorldCentricTabColorScale() const
{
	return FColor::Red;
}

UCombatFlipbook* FCombatFlipbookEditor::GetCombatFlipbookBeingEdited()
{
	return CombatFlipbookBeingEdited;
}

UPaperFlipbook* FCombatFlipbookEditor::GetFlipbookBeingEdited() const
{
	return CombatFlipbookBeingEdited->TargetFlipbook;
}

FCombatFrames* FCombatFlipbookEditor::CreateKeyFrameDataOnCurrentFrame()
{
	const int32 curFrame = GetCurrentFrame();

	TArray<FCombatFrames>& keyFrameArray = GetCombatFlipbookBeingEdited()->CombatFramesArray;

	while (!keyFrameArray.IsValidIndex(curFrame))
	{
		if (curFrame == INDEX_NONE)
		{
			return nullptr;
		}

		const int32 index = keyFrameArray.AddDefaulted();

		// add one frame instruction just to be sure
		keyFrameArray[index].CollisionDataArray.AddDefaulted();
	}

	return &keyFrameArray[curFrame];
}

void FCombatFlipbookEditor::CreateKeyFrameInstructionOnCurrentFrame()
{
	const int32 curFrame = GetCurrentFrame();

	if (curFrame == INDEX_NONE)
	{
		return;
	}

	TArray<FCombatFrames>& keyFrameArray = GetCombatFlipbookBeingEdited()->CombatFramesArray;

	if (keyFrameArray.IsValidIndex(curFrame))
	{
		keyFrameArray[curFrame].CollisionDataArray.AddDefaulted();
	}
}

void FCombatFlipbookEditor::ExtendToolbar()
{
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			const FCombatSpriteGeometryEditCommands& GeometryCommands =
				FCombatSpriteGeometryEditCommands::Get();
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
	    MenuExtender->AddMenuBarExtension("Flipbook Data Editor", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FCombatFlipbookEditor::MakePulldownMenu));
	    LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}*/

	//ICombat2DModule* Combat2DModule = &FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
	//AddToolbarExtender(Combat2DModule->GetSpriteEditorToolBarExtensibilityManager()->GetAllExtenders());
}

TSharedRef<SDockTab> FCombatFlipbookEditor::SpawnTab_Viewport(
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
		                                                           &FCombatFlipbookEditor::GetPlaybackPosition)
                                     .NumOfKeys(
		                                                           this,
		                                                           &FCombatFlipbookEditor::GetTotalFrameCountPlusOne)
                                     .SequenceLength(
		                                                           this,
		                                                           &FCombatFlipbookEditor::GetTotalSequenceLength)
                                     .OnValueChanged(
		                                                           this,
		                                                           &FCombatFlipbookEditor::SetPlaybackPosition)
                                     //		.OnBeginSliderMovement(this, &SAnimationScrubPanel::OnBeginSliderMovement)
                                     //		.OnEndSliderMovement(this, &SAnimationScrubPanel::OnEndSliderMovement)
                                     .OnClickedForwardPlay(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_Forward)
                                     .OnClickedForwardStep(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_Forward_Step)
                                     .OnClickedForwardEnd(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_Forward_End)
                                     .OnClickedBackwardPlay(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_Backward)
                                     .OnClickedBackwardStep(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_Backward_Step)
                                     .OnClickedBackwardEnd(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_Backward_End)
                                     .OnClickedToggleLoop(
		                                                           this,
		                                                           &FCombatFlipbookEditor::OnClick_ToggleLoop)
                                     .OnGetLooping(
		                                                           this, &FCombatFlipbookEditor::IsLooping)
                                     .OnGetPlaybackMode(
		                                                           this,
		                                                           &FCombatFlipbookEditor::GetPlaybackMode)
                                     .ViewInputMin(
		                                                           this,
		                                                           &FCombatFlipbookEditor::GetViewRangeMin)
                                     .ViewInputMax(
		                                                           this,
		                                                           &FCombatFlipbookEditor::GetViewRangeMax)
                                     .OnSetInputViewRange(
		                                                           this,
		                                                           &FCombatFlipbookEditor::SetViewRange)
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
				SNew(SCombatFlipbookTimeline, GetToolkitCommands())
                 .CombatFlipbookBeingEdited(GetCombatFlipbookBeingEdited())
                 .OnSelectionChanged(this, &FCombatFlipbookEditor::SetSelection)
                 .PlayTime(this, &FCombatFlipbookEditor::GetPlaybackPosition)
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

TSharedRef<SDockTab> FCombatFlipbookEditor::SpawnTab_Details(
	const FSpawnTabArgs& Args)
{
	TSharedPtr<FCombatFlipbookEditor> FlipbookEditorPtr = SharedThis(this);

	// Spawn the tab
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
	[
		SNew(SCombatFlipbookDetailsTab, FlipbookEditorPtr)
	];
}

TSharedRef<SDockTab> FCombatFlipbookEditor::SpawnTab_Geometry(
	const FSpawnTabArgs& Args)
{
	TSharedPtr<FCombatFlipbookEditor> FlipbookEditorPtr = SharedThis(this);

	FrameCollisionDataTab = SNew(SFrameCollisionDataTab, FlipbookEditorPtr);

	// Spawn the tab
	return SNew(SDockTab)
        .Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Geometry"))
        .Label(LOCTEXT("GeometryTab_Title", "Geometry"))
	[
		FrameCollisionDataTab.ToSharedRef()
	];
}

void FCombatFlipbookEditor::CreateModeToolbarWidgets(
	FToolBarBuilder& IgnoredBuilder)
{
	FToolBarBuilder ToolbarBuilder(ViewportPtr->GetCommandList(),
	                               FMultiBoxCustomization::None);
	ToolbarBuilder.AddToolBarButton(
		FCombatFlipbookEditorCommands::Get().EnterViewMode);
	ToolbarBuilder.AddToolBarButton(
		FCombatFlipbookEditorCommands::Get().EnterSourceRegionEditMode);
	ToolbarBuilder.AddToolBarButton(
		FCombatFlipbookEditorCommands::Get().EnterCollisionEditMode);
	ToolbarBuilder.AddToolBarButton(
		FCombatFlipbookEditorCommands::Get().EnterRenderingEditMode);
	AddToolbarWidget(ToolbarBuilder.MakeWidget());
}

TSharedRef<SDockTab> FCombatFlipbookEditor::SpawnPropertiesTab(
	const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCombatFlipbookEditorTabs::DetailsID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
	[
		DetailsView.ToSharedRef()
	];
}

void FCombatFlipbookEditor::AddKeyFrameAtCurrentTime()
{
	const FScopedTransaction Transaction(
		LOCTEXT("InsertKeyFrameTransaction", "Insert Key Frame"));
	CombatFlipbookBeingEdited->Modify();

	const float CurrentTime = GetPlaybackPosition();
	const int32 KeyFrameIndex = GetFlipbookBeingEdited()->GetKeyFrameIndexAtTime(
		CurrentTime);
	const int32 ClampedIndex = FMath::Clamp<int32>(
		KeyFrameIndex, 0, GetFlipbookBeingEdited()->GetNumFrames());

	FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());
	FPaperFlipbookKeyFrame NewFrame;
	EditLock.KeyFrames.Insert(NewFrame, ClampedIndex);
}

void FCombatFlipbookEditor::AddNewKeyFrameAtEnd()
{
	const FScopedTransaction Transaction(
		LOCTEXT("AddKeyFrameTransaction", "Add Key Frame"));
	GetFlipbookBeingEdited()->Modify();

	FScopedFlipbookMutator EditLock(GetFlipbookBeingEdited());

	FPaperFlipbookKeyFrame& NewFrame = *new(EditLock.KeyFrames)
		FPaperFlipbookKeyFrame();
}

void FCombatFlipbookEditor::AddNewKeyFrameBefore()
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

void FCombatFlipbookEditor::AddNewKeyFrameAfter()
{
	UPaperFlipbook* FlipbookBeingEdited = CombatFlipbookBeingEdited->TargetFlipbook;

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


void FCombatFlipbookEditor::SetSelection(int32 NewSelection)
{
	CurrentSelectedKeyframe = NewSelection;
}

bool FCombatFlipbookEditor::HasValidSelection() const
{
	return GetFlipbookBeingEdited()->
		IsValidKeyFrameIndex(CurrentSelectedKeyframe);
}

FReply FCombatFlipbookEditor::OnClick_Forward()
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

FReply FCombatFlipbookEditor::OnClick_Forward_Step()
{
	GetPreviewComponent()->Stop();
	SetCurrentFrame(GetCurrentFrame() + 1);
	return FReply::Handled();
}

FReply FCombatFlipbookEditor::OnClick_Forward_End()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	PreviewComponent->Stop();
	PreviewComponent->SetPlaybackPosition(PreviewComponent->GetFlipbookLength(),
	                                      /*bFireEvents=*/ false);
	return FReply::Handled();
}

FReply FCombatFlipbookEditor::OnClick_Backward()
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

FReply FCombatFlipbookEditor::OnClick_Backward_Step()
{
	GetPreviewComponent()->Stop();
	SetCurrentFrame(GetCurrentFrame() - 1);
	return FReply::Handled();
}

FReply FCombatFlipbookEditor::OnClick_Backward_End()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	PreviewComponent->Stop();
	PreviewComponent->SetPlaybackPosition(0.0f, /*bFireEvents=*/ false);
	return FReply::Handled();
}

FReply FCombatFlipbookEditor::OnClick_ToggleLoop()
{
	UPaperFlipbookComponent* PreviewComponent = GetPreviewComponent();
	PreviewComponent->SetLooping(!PreviewComponent->IsLooping());
	return FReply::Handled();
}

EPlaybackMode::Type FCombatFlipbookEditor::GetPlaybackMode() const
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

uint32 FCombatFlipbookEditor::GetTotalFrameCount() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0 : GetFlipbookBeingEdited()->GetNumFrames();
}

uint32 FCombatFlipbookEditor::GetTotalFrameCountPlusOne() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0 : GetFlipbookBeingEdited()->GetNumFrames() + 1;
}

float FCombatFlipbookEditor::GetTotalSequenceLength() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0.f : GetFlipbookBeingEdited()->GetTotalDuration();
}

float FCombatFlipbookEditor::GetPlaybackPosition() const
{
	return GetFlipbookBeingEdited() == nullptr ? 0.f : GetPreviewComponent()->GetPlaybackPosition();
}

void FCombatFlipbookEditor::SetPlaybackPosition(float NewTime)
{
	NewTime = FMath::Clamp<float>(NewTime, 0.0f, GetTotalSequenceLength());

	GetPreviewComponent()->SetPlaybackPosition(NewTime, /*bFireEvents=*/ false);
}

bool FCombatFlipbookEditor::IsLooping() const
{
	return GetPreviewComponent()->IsLooping();
}

float FCombatFlipbookEditor::GetViewRangeMin() const
{
	return ViewInputMin;
}

float FCombatFlipbookEditor::GetViewRangeMax() const
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

void FCombatFlipbookEditor::SetViewRange(float NewMin, float NewMax)
{
	ViewInputMin = FMath::Max<float>(NewMin, 0.0f);
	ViewInputMax = FMath::Min<float>(NewMax, GetTotalSequenceLength());
}

float FCombatFlipbookEditor::GetFramesPerSecond() const
{
	return CombatFlipbookBeingEdited->TargetFlipbook->GetFramesPerSecond();
}

int32 FCombatFlipbookEditor::GetCurrentFrame() const
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

void FCombatFlipbookEditor::SetCurrentFrame(int32 NewIndex)
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

void FCombatFlipbookEditor::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook,
                                                   const TSharedPtr<FUICommandList>& CommandList,
                                                   EExtensionHook::Position position)
{
	MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FCombatFlipbookEditor::MakePulldownMenu(FMenuBarBuilder& menuBuilder)
{
	menuBuilder.AddPullDownMenu(
		FText::FromString("Example"),
		FText::FromString("Open the Example menu"),
		FNewMenuDelegate::CreateRaw(this, &FCombatFlipbookEditor::FillPulldownMenu),
		"Example",
		FName(TEXT("ExampleMenu"))
	);
}

void FCombatFlipbookEditor::FillPulldownMenu(FMenuBuilder& menuBuilder)
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
