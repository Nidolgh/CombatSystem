#include "CombatSystemEditorModule.h"

#include "AnimNotifyState_CombatSystemCollision.h"
#include "AssetToolsModule.h"
#include "CombatSystemCollisionDetails.h"
#include "CombatSystemEditorModeCommands.h"
#include "CombatSystemEditorStyle.h"
#include "EditorModeManager.h"
#include "EditorModeRegistry.h"
#include "IAnimationEditorModule.h"
#include "MovesetAssetTypeActions.h"
#include "NotifyCollisionSelectionEditMode.h"
#include "PersonaModule.h"
#include "SCombatSystemSectionsPanel.h"
#include "Animation/EditorNotifyObject.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

static const FName CombatSystemAnimationWindowTabName("TestStandaloneWindow");
const FEditorModeID FCombatSystemEditModes::EM_CombatSystemID(TEXT("EM_CombatSystemID"));

#define LOCTEXT_NAMESPACE "FCombatSystemModule"

void FCombatSystemEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FCombatSystemEditorModule::OnPostEngineInit);
}

void FCombatSystemEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	
	FCombatSystemEditorStyle::Shutdown();

	FCombatSystemEditorModeCommands::Unregister();

	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return;
	}
	
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(MovesetAssetTypeActionsAssetTypeActions.ToSharedRef());
	FGlobalTabmanager::Get()->UnregisterTabSpawner(CombatSystemAnimationWindowTabName);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(UEditorAnimBaseObj::StaticClass()->GetFName());
}

void FCombatSystemEditorModule::OnPostEngineInit()
{
	FCombatSystemEditorStyle::Initialize();
	FCombatSystemEditorStyle::ReloadTextures();

	// Custom Window
	FCombatSystemEditorModeCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FCombatSystemEditorModeCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FCombatSystemEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	//UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCombatSystemEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterTabSpawner(CombatSystemAnimationWindowTabName, FOnSpawnTab::CreateRaw(this, &FCombatSystemEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTestStandaloneWindowTabTitle", "TestStandaloneWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Custom asset
	FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("CombatSystem"), FText::FromString("Combat System"));
	
	MovesetAssetTypeActionsAssetTypeActions = MakeShared<FMovesetAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(MovesetAssetTypeActionsAssetTypeActions.ToSharedRef());

	// Register the editor modes
	FEditorModeRegistry::Get().RegisterMode<FNotifyCollisionSelectionEditMode>(FCombatSystemEditModes::EM_CombatSystemID, LOCTEXT("NotifyCollisionSelectionEditMode", "Notify Collision Selection"), FSlateIcon(), false);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// Using UEditorNotifyObject as our class name causes the entire details panel to lose data. I guess its overwriting another custom details panel? Weird though
	PropertyModule.RegisterCustomClassLayout(UEditorAnimBaseObj::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FCombatSystemCollisionDetails::MakeInstance));
	
	ExtendToolbar();
}

void FCombatSystemEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(CombatSystemAnimationWindowTabName);
}

void FCombatSystemEditorModule::ExtendToolbar() const
{
	FModuleManager::Get().LoadModule("AnimationEditor");

	// AnimationEditor.cpp ExtendToolbar
	IAnimationEditorModule& AnimationEditorModule = FModuleManager::GetModuleChecked<IAnimationEditorModule>("AnimationEditor");

	TSharedRef<FExtender> Extender = MakeShareable(new FExtender);
	// Add the sprite actions sub-menu extender
	Extender->AddToolBarExtension(
		"Editing",
		EExtensionHook::After,
		PluginCommands,
		FToolBarExtensionDelegate::CreateLambda(
				[this](FToolBarBuilder& ToolbarBuilder) {
					
					ToolbarBuilder.BeginSection("CustomMine");

					ToolbarBuilder.AddToolBarButton(FCombatSystemEditorModeCommands::Get().OpenPluginWindow);

					ToolbarBuilder.AddEditableText( LOCTEXT("EditableItem", "Editable Item" ), LOCTEXT("EditableItem_ToolTip", "You can edit this item's text" ), FSlateIcon(), LOCTEXT("DefaultEditableText", "Edit Me!" )) ;

					ToolbarBuilder.EndSection();
				})
	);
	
	AnimationEditorModule.GetToolBarExtensibilityManager()->AddExtender(Extender);
	
	FPersonaModule& PersonaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	PersonaModule.OnRegisterTabs().AddLambda([](FWorkflowAllowedTabSet& TabFactories, TSharedPtr<FAssetEditorToolkit> InHostingApp)
	{
		TabFactories.RegisterFactory(MakeShareable(new FCombatSystemSectionsSummoner(InHostingApp)));
	});
}

void FCombatSystemEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("AssetEditor.AnimationEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddSeparator("CombatSystem");
			Section.AddMenuEntryWithCommandList(FCombatSystemEditorModeCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FCombatSystemEditorModeCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

TSharedRef<SDockTab> FCombatSystemEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FCombatSystemEditorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("CombatSystemEditorModule.cpp"))
		);
	
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			// Put your tab content here!
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Lambda([this]() -> FReply
				{
					const FEditorViewportClient* client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
					client->GetModeTools()->ActivateMode("EM_CombatSystemID");
					
					return FReply::Handled();
				})
				.Text(LOCTEXT("ExtendToolbarButton", "Activate Combat System Mode"))
			].AutoHeight()
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Lambda([this]() -> FReply
				{
					const FEditorViewportClient* client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
					client->GetModeTools()->ActivateDefaultMode();
					
					return FReply::Handled();
				})
				.Text(LOCTEXT("ExtendToolbarButton", "Deactivate Combat System Mode"))
				]
		];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCombatSystemEditorModule, CombatSystemEditorMode)