#include "SCombatSystemSectionsPanel.h"

#include "AnimationToolMenuContext.h"
#include "CombatSystemEditorModule.h"
#include "EditorModeManager.h"
#include "IAnimationEditor.h"
#include "IPersonaToolkit.h"
#include "PersonaModule.h"
#include "Animation/AnimMontage.h"

#define LOCTEXT_NAMESPACE "CombatSystemSectionsPanel"

const FName CombatSystemSectionsSummonerID("CombatSystemSectionsSummoner");

FCombatSystemSectionsSummoner::FCombatSystemSectionsSummoner(TSharedPtr<FAssetEditorToolkit> InHostingApp)
	: FWorkflowTabFactory(CombatSystemSectionsSummonerID, InHostingApp)
{
	TabLabel = LOCTEXT("CombatSystemSectionsTitle", "Combat System Sections");
	// TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Persona.Tabs.AnimSlotManager");

	bIsSingleton = true;
	ViewMenuDescription = LOCTEXT("CombatSystemSectionsTitle_Description", "Combat System Collision Editor");
	ViewMenuTooltip = LOCTEXT("CombatSystemSectionsTitle_Tooltip", "Combat System Collision Editor");
}

TSharedRef<SWidget> FCombatSystemSectionsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SCombatSystemSectionsPanel);
}

TSharedPtr<SToolTip> FCombatSystemSectionsSummoner::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::CreateTabToolTipWidget(Info);
}

SCombatSystemSectionsPanel::~SCombatSystemSectionsPanel()
{
	if(GEditor)
	{
		GEditor->RegisterForUndo(this);
	}
}

void SCombatSystemSectionsPanel::Construct(const FArguments& InArgs)
{
	//WeakPersonaToolkit = InPersonaToolkit;

	// UAnimMontage* Montage = Cast<UAnimMontage>(WeakPersonaToolkit.Pin()->GetAnimationAsset());

	if(GEditor)
	{
		GEditor->RegisterForUndo(this);
	}
	
	this->ChildSlot
	[
		SAssignNew( PanelArea, SBorder )
		.BorderImage( FAppStyle::GetBrush("NoBorder") )
		.ColorAndOpacity( FLinearColor::White )
	];

	Update();
}

void SCombatSystemSectionsPanel::ManageActiveEditMode(FEditorModeID ModeToActivate, FEditorModeID ModeToDeactivate)
{
	auto ViewportClients = GEditor->GetAllViewportClients();
	for (const auto ViewportClient : ViewportClients)
	{
		const FEdMode* EDMode = ViewportClient->GetModeTools()->GetActiveMode(ModeToDeactivate);
		if (EDMode != nullptr)
		{
			ViewportClient->GetModeTools()->DeactivateMode(ModeToDeactivate);
			
			ViewportClient->GetModeTools()->SetDefaultMode(ModeToActivate);
			ViewportClient->GetModeTools()->ActivateDefaultMode();
		}
	}
}

void SCombatSystemSectionsPanel::Update()
{
	TSharedPtr<SVerticalBox> ContentArea;
	PanelArea->SetContent(
		SAssignNew( ContentArea, SVerticalBox )
		);
	
	ContentArea->ClearChildren();

	const FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FCombatSystemEditorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("CombatSystemEditorModule.cpp"))
		);

	ContentArea->AddSlot()
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
					ManageActiveEditMode(FCombatSystemEditModes::EM_CombatSystemID, FPersonaEditModes::SkeletonSelection);
					
					return FReply::Handled();
				})
				.Text(LOCTEXT("ActivateCombatSystemModeButton", "Activate Combat System Mode"))
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Lambda([this]() -> FReply
				{
					ManageActiveEditMode(FPersonaEditModes::SkeletonSelection, FCombatSystemEditModes::EM_CombatSystemID);

					return FReply::Handled();
				})
				.Text(LOCTEXT("DeactivateCombatSystemModeButton", "Deactivate Combat System Mode"))
				]
	];
}
