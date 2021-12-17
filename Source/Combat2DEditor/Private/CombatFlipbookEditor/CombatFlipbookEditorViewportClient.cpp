// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatFlipbookEditorViewportClient.h"

#include "Engine/CollisionProfile.h"

#include "CanvasItem.h"
#include "Utils.h"
#include "AssetEditorModeManager.h"
#include "IContentBrowserSingleton.h"
#include "CanvasTypes.h"
#include "CombatFlipbookEditorSettings.h"
#include "CombatFlipbookEditor.h"

#include "PaperFlipbookComponent.h"
#include "CombatGeometryCollisionBuilder.h"

#include "CombatFlipbook.h"

#define LOCTEXT_NAMESPACE "CombatFlipbookEditor"

//////////////////////////////////////////////////////////////////////////
// FSelectionTypes

const FName FSelectionTypes::GeometryShape(TEXT("GeometryShape"));
const FName FSelectionTypes::Vertex(TEXT("Vertex"));
const FName FSelectionTypes::Edge(TEXT("Edge"));
const FName FSelectionTypes::Pivot(TEXT("Pivot"));
const FName FSelectionTypes::SourceRegion(TEXT("SourceRegion"));

//////////////////////////////////////////////////////////////////////////
// Sprite editing constants

namespace SpriteEditingConstants
{
	// Tint the source texture darker to help distinguish it from the sprite being edited
	const FLinearColor SourceTextureDarkTintColor(0.05f, 0.05f, 0.05f, 1.0f);

	// Note: MinMouseRadius must be greater than MinArrowLength
	const FLinearColor BakedCollisionLineRenderColor(1.0f, 1.0f, 0.0f, 0.25f);
	const FLinearColor BakedCollisionRenderColor(1.0f, 1.0f, 0.0f, 0.5f);
	const float BakedCollisionVertexSize = 3.0f;

	const FLinearColor SourceRegionBoundsColor(1.0f, 1.0f, 1.0f, 0.8f);
	const FLinearColor SourceRegionRelatedBoundsColor(0.3f, 0.3f, 0.3f, 0.8f);
	const FLinearColor SourceRegionRelatedSpriteNameColor(0.6f, 0.6f, 0.6f, 0.8f);

	const FLinearColor CollisionShapeColor(0.0f, 0.7f, 1.0f, 1.0f);
	const FLinearColor RenderShapeColor(1.0f, 0.2f, 0.0f, 1.0f);
	const FLinearColor SubtractiveRenderShapeColor(0.0f, 0.2f, 1.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////
// FCombatFlipbookEditorViewportClient

FCombatFlipbookEditorViewportClient::FCombatFlipbookEditorViewportClient(TWeakPtr<FCombatFlipbookEditor> InSpriteEditor, TWeakPtr<class SEditorViewport> InCombatFlipbookEditorViewportPtr)
	:
	FEditorViewportClient(new FAssetEditorModeManager(), nullptr, InCombatFlipbookEditorViewportPtr)
	, bManipulating(false)
	, CombatFlipbookEditorPtr(InSpriteEditor)
	, CombatFlipbookEditorViewportPtr(InCombatFlipbookEditorViewportPtr)
{
	check(CombatFlipbookEditorPtr.IsValid());

	// The tile map editor fully supports mode tools and isn't doing any incompatible stuff with the Widget
	Widget->SetUsesEditorModeTools(ModeTools);

	CombatFlipbookBeingEdited = InSpriteEditor.Pin()->GetCombatFlipbookBeingEdited();
	FlipbookBeingEditedLastFrame = CombatFlipbookBeingEdited.Get()->TargetFlipbook;
	PreviewScene = &OwnedPreviewScene;
	((FAssetEditorModeManager*)ModeTools)->SetPreviewScene(PreviewScene);

	SetRealtime(true);

	// Create a render component for the sprite being edited
	AnimatedRenderComponent = NewObject<UPaperFlipbookComponent>();
	AnimatedRenderComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	AnimatedRenderComponent->SetFlipbook(CombatFlipbookBeingEdited.Get()->TargetFlipbook);
	AnimatedRenderComponent->UpdateBounds();
	AnimatedRenderComponent->Stop();

	PreviewScene->AddComponent(AnimatedRenderComponent.Get(), FTransform::Identity);

	bShowPivot = true;

	DrawHelper.bDrawGrid = 0;

	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetCompositeEditorPrimitives(true);

	SetViewModes(VMI_Lit, VMI_Lit);

	// keep direction in mind
	const ELevelViewportType NewViewportType = LVT_OrthoXZ;
	SetViewportType(NewViewportType);
}

void FCombatFlipbookEditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);

	GeometryEditMode->DrawHUD(this, Viewport, &View, &Canvas);

	const bool bIsHitTesting = Canvas.IsHitTesting();
	if (!bIsHitTesting)
	{
		Canvas.SetHitProxy(nullptr);
	}

	int32 YPos = 42;

	// todo combine help text
	//static const FText FlipbookHelpStr = LOCTEXT("FlipbookEditHelp", "Flipbook editor\n\nAdd keys using the toolbar or by drag-dropping sprite assets\nChange the timeline scale using Ctrl+MouseWheel\nEdit keys using the handles and right-click menu\nReorder keys by dragging and dropping");
	static const FText FlipbookHelpStr = LOCTEXT("FlipbookEditHelp", "");

	// Display tool help
	{
		FCanvasTextItem TextItem(FVector2D(6, YPos), FlipbookHelpStr, GEngine->GetSmallFont(), FLinearColor::White);
		TextItem.EnableShadow(FLinearColor::Black);
		TextItem.Draw(&Canvas);
		YPos += 36;
	}
}

void FCombatFlipbookEditorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	GeometryEditMode->Render(View, Viewport, PDI);

	if (bShowPivot && AnimatedRenderComponent.IsValid())
	{
		FUnrealEdUtils::DrawWidget(View, PDI, AnimatedRenderComponent->GetComponentTransform().ToMatrixWithScale(), 0, 0, EAxisList::Screen, EWidgetMovementMode::WMM_Translate);
	}
}


void FCombatFlipbookEditorViewportClient::Tick(float DeltaSeconds)
{
	GeometryEditMode->Tick(this, DeltaSeconds);

	HandleKeyFrameChange();
	HandleCollisionDataIndexChange();
	
	RegenerateCombatFramesBodySetups();

	if (AnimatedRenderComponent.IsValid())
	{		
		UPaperFlipbook* Flipbook = CombatFlipbookBeingEdited.Get()->TargetFlipbook;

		if (Flipbook != FlipbookBeingEditedLastFrame.Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("Changing"));

			AnimatedRenderComponent->SetFlipbook(Flipbook);
			AnimatedRenderComponent->UpdateBounds();
			FlipbookBeingEditedLastFrame = Flipbook;
		}
	}
	
	{
		// Zoom in on the sprite
		//@TODO: Fix this properly so it doesn't need to be deferred, or wait for the viewport to initialize
		FIntPoint Size = Viewport->GetSizeXY();
		if (bDeferZoomToSprite && (Size.X > 0) && (Size.Y > 0))
		{
			FBox BoundsToFocus = GetDesiredFocusBounds();
			if (ViewportType != LVT_Perspective)
			{
				TGuardValue<ELevelViewportType> SaveViewportType(ViewportType, LVT_Perspective);
				FocusViewportOnBox(BoundsToFocus, bDeferZoomToSpriteIsInstant);
			}

			FocusViewportOnBox(BoundsToFocus, bDeferZoomToSpriteIsInstant);
			bDeferZoomToSprite = false;
		}

	}

	FEditorViewportClient::Tick(DeltaSeconds);
	
	if (!GIntraFrameDebuggingGameThread)
	{
		OwnedPreviewScene.GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

void FCombatFlipbookEditorViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	const FViewportClick Click(&View, this, Key, Event, HitX, HitY);
	const bool bIsCtrlKeyDown = Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);
	const bool bIsShiftKeyDown = Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
	const bool bIsAltKeyDown = Viewport->KeyState(EKeys::LeftAlt) || Viewport->KeyState(EKeys::RightAlt);
	bool bHandled = false;

	GeometryEditMode->HandleClick(this, HitProxy, Click);
}

bool FCombatFlipbookEditorViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	bool bHandled = false;
	FInputEventState InputState(InViewport, Key, Event);

	GeometryEditMode->InputKey(this, InViewport, Key, Event);

	// Pass keys to standard controls, if we didn't consume input
	return (bHandled) ? true : FEditorViewportClient::InputKey(InViewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
}

FLinearColor FCombatFlipbookEditorViewportClient::GetBackgroundColor() const
{
	return GetDefault<UCombatFlipbookEditorSettings>()->BackgroundColor;
}

void FCombatFlipbookEditorViewportClient::TrackingStarted(const FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge)
{
	//@TODO: Should push this into FEditorViewportClient
	// Begin transacting.  Give the current editor mode an opportunity to do the transacting.
	const bool bTrackingHandledExternally = ModeTools->StartTracking(this, Viewport);

	if (!bManipulating && bIsDraggingWidget && !bTrackingHandledExternally)
	{
		BeginTransaction(LOCTEXT("ModificationInViewportTransaction", "Modification in Viewport"));
		bManipulating = true;
		bManipulationDirtiedSomething = false;
	}
}

void FCombatFlipbookEditorViewportClient::TrackingStopped()
{
	// Stop transacting.  Give the current editor mode an opportunity to do the transacting.
	const bool bTransactingHandledByEditorMode = ModeTools->EndTracking(this, Viewport);

	if (bManipulating && !bTransactingHandledByEditorMode)
	{
		EndTransaction();
		bManipulating = false;
	}
}

FVector2D FCombatFlipbookEditorViewportClient::SelectedItemConvertWorldSpaceDeltaToLocalSpace(const FVector& WorldSpaceDelta) const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? FVector2D(0.f) : Sprite->ConvertWorldSpaceDeltaToTextureSpace(WorldSpaceDelta);
}

FVector2D FCombatFlipbookEditorViewportClient::WorldSpaceToTextureSpace(const FVector& SourcePoint) const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? FVector2D(0.f) : Sprite->ConvertWorldSpaceToTextureSpace(SourcePoint);
}

FVector FCombatFlipbookEditorViewportClient::TextureSpaceToWorldSpace(const FVector2D& SourcePoint) const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? FVector(0.f) : Sprite->ConvertTextureSpaceToWorldSpace(SourcePoint);
}

float FCombatFlipbookEditorViewportClient::SelectedItemGetUnitsPerPixel() const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? 0.f : Sprite->GetUnrealUnitsPerPixel();
}

void FCombatFlipbookEditorViewportClient::BeginTransaction(const FText& SessionName)
{
	if (ScopedTransaction == nullptr)
	{
		ScopedTransaction = new FScopedTransaction(SessionName);

		UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
		Sprite->Modify();
	}
}

void FCombatFlipbookEditorViewportClient::MarkTransactionAsDirty()
{
	bManipulationDirtiedSomething = true;
	Invalidate();
}

void FCombatFlipbookEditorViewportClient::EndTransaction()
{
	bManipulationDirtiedSomething = false;

	if (ScopedTransaction != nullptr)
	{
		delete ScopedTransaction;
		ScopedTransaction = nullptr;
	}
}

void FCombatFlipbookEditorViewportClient::InvalidateViewportAndHitProxies()
{
	Viewport->Invalidate();
}

void FCombatFlipbookEditorViewportClient::ActivateEditMode()
{
	// Activate the sprite geometry edit mode
	ModeTools->SetToolkitHost(CombatFlipbookEditorPtr.Pin()->GetToolkitHost());
	ModeTools->SetDefaultMode(FCombatSpriteGeometryEditMode::EM_CombatSpriteGeometry);
	ModeTools->ActivateDefaultMode();

	//setup the geometry edit mode
	GeometryEditMode = (FCombatSpriteGeometryEditMode*)ModeTools->GetActiveMode(FCombatSpriteGeometryEditMode::EM_CombatSpriteGeometry);
	GeometryEditMode->SetEditorContext(this);
	GeometryEditMode->SetModeTools(GetModeTools());
	GeometryEditMode->BindCommands(CombatFlipbookEditorViewportPtr.Pin()->GetCommandList());

	GeometryEditMode->SetGeometryColors(
		FLinearColor(1.f, 1.f, 1.f, 1.f), 
		FLinearColor(1.f, 0.f, 1.f, 1.f));

	ModeTools->SetWidgetMode(FWidget::WM_Translate);
}

void FCombatFlipbookEditorViewportClient::RequestFocusOnSelection(bool bInstant)
{
	bDeferZoomToSprite = true;
	bDeferZoomToSpriteIsInstant = bInstant;
}

UPaperSprite* FCombatFlipbookEditorViewportClient::GetSpriteOnCurrentFrame() const
{
	const int32 curFrame = CombatFlipbookEditorPtr.Pin().Get()->GetCurrentFrame();
	return CombatFlipbookBeingEdited.Get()->TargetFlipbook->GetSpriteAtFrame(curFrame);
}

FCombatFrame* FCombatFlipbookEditorViewportClient::GetCombatFrameDataOnCurrentFrame() const
{
	const int32 curFrame = CombatFlipbookEditorPtr.Pin().Get()->GetCurrentFrame();

	if (CombatFlipbookBeingEdited.Get()->CombatFramesArray.IsValidIndex(curFrame))
	{
		return &CombatFlipbookBeingEdited.Get()->CombatFramesArray[curFrame];
	}

	return nullptr;
}

UPaperFlipbookComponent * FCombatFlipbookEditorViewportClient::GetPreviewComponent() const
{
	return AnimatedRenderComponent.Get();
}

void FCombatFlipbookEditorViewportClient::RegenerateCombatFramesBodySetups() const
{
	UCombatFlipbook* ActiveCombatFlipbook = CombatFlipbookBeingEdited.Get();
	
	if (ActiveCombatFlipbook == nullptr)
		return;
	
	if (ActiveCombatFlipbook->GetIsDirty())
	{
		for (FCombatFrame& CombatFrame : ActiveCombatFlipbook->CombatFramesArray)
		{
			for (FCombatFrameCollisionData& FrameCollisionData : CombatFrame.CollisionDataArray)
			{
				FSpriteGeometryCollection& GeometryCollection = FrameCollisionData.CollisionGeometry;

				UBodySetup* OldBodySetup = FrameCollisionData.GeneratedBodySetup;
				UBodySetup* NewBodySetup = NewObject<UBodySetup>();
				
				FrameCollisionData.GeneratedBodySetup = NewBodySetup;

				check(FrameCollisionData.GeneratedBodySetup);
				NewBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;

				// Clean up the geometry (converting polygons back to bounding boxes, etc...)
				GeometryCollection.ConditionGeometry();

				// Take the geometry and add it to the body setup
				FCombatGeometryCollisionBuilder CollisionBuilder(FrameCollisionData.GeneratedBodySetup);
				CollisionBuilder.SetUnrealUnitsPerPixel(ActiveCombatFlipbook->UnrealUnitsPerPixel);
				CollisionBuilder.ProcessGeometry(GeometryCollection);
				CollisionBuilder.Finalize();
	
				// Copy across or initialize the only editable property we expose on the body setup
				if (OldBodySetup != nullptr)
				{
					NewBodySetup->DefaultInstance.CopyBodyInstancePropertiesFrom(&(OldBodySetup->DefaultInstance));
				}
				else
				{
					NewBodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
				}
			}
		}
	}
	
	ActiveCombatFlipbook->SetIsDirty(false);
}

FBox FCombatFlipbookEditorViewportClient::GetDesiredFocusBounds() const
{
	return AnimatedRenderComponent->Bounds.GetBox();
}

void FCombatFlipbookEditorViewportClient::HandleKeyFrameChange()
{
	CurrentKeyFrameData = CombatFlipbookEditorPtr.Pin()->CreateKeyFrameDataOnCurrentFrame();
	if (CurrentKeyFrameData != nullptr && CurrentKeyFrameData != LastKeyFrameData) 
	{
		const int32 curFrame = CombatFlipbookEditorPtr.Pin()->GetCurrentFrame();
		FSpriteGeometryCollection* GeometryCollectionInstruction = CurrentKeyFrameData->CollisionDataArray.IsValidIndex(curFrame) ? &CurrentKeyFrameData->CollisionDataArray[0].CollisionGeometry : nullptr;

		if (CurrentKeyFrameData->CollisionDataArray.Num() != 0)
		{
			GeometryEditMode->SetCurrentCombatFrame(CurrentKeyFrameData);
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("No frame instruction collection set"));
		}

		LastKeyFrameData = CurrentKeyFrameData;
	}
}

void FCombatFlipbookEditorViewportClient::HandleCollisionDataIndexChange()
{
	int32 InsID = CombatFlipbookEditorPtr.Pin()->GetGeoPropTabBody()->GetButtonFrameID();
	if (InsID != GeometryEditMode->GetCollDataToEditIndex())
	{
		GeometryEditMode->SetCollDataToEditIndex(InsID);
	}
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
