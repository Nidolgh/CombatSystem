// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipbookDataEditorViewportClient.h"

#include "Engine/CollisionProfile.h"

#include "CanvasItem.h"
#include "Utils.h"
#include "AssetEditorModeManager.h"
#include "IContentBrowserSingleton.h"
#include "CanvasTypes.h"
#include "FlipbookDataEditor.h"
#include "FlipbookDataEditorSettings.h"

#include "PaperFlipbookComponent.h"
#include "SpriteGeometryEditCommands.h"

#define LOCTEXT_NAMESPACE "FlipbookDataEditor"

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
// FFlipbookDataEditorViewportClient

FFlipbookDataEditorViewportClient::FFlipbookDataEditorViewportClient(TWeakPtr<FFlipbookDataEditor> InSpriteEditor, TWeakPtr<class SEditorViewport> InFlipbookDataEditorViewportPtr)
	:
	FEditorViewportClient(new FAssetEditorModeManager(), nullptr, InFlipbookDataEditorViewportPtr)
	, FlipbookDataEditorPtr(InSpriteEditor)
{
	check(FlipbookDataEditorPtr.IsValid());

	// The tile map editor fully supports mode tools and isn't doing any incompatible stuff with the Widget
	Widget->SetUsesEditorModeTools(ModeTools);

	FlipbookDataBeingEdited = InSpriteEditor.Pin()->GetFlipbookDataBeingEdited();
	FlipbookBeingEditedLastFrame = FlipbookDataBeingEdited.Get()->TargetFlipbook;
	PreviewScene = &OwnedPreviewScene;
	((FAssetEditorModeManager*)ModeTools)->SetPreviewScene(PreviewScene);
	
	SetRealtime(true);

	// Create a render component for the sprite being edited
	AnimatedRenderComponent = NewObject<UPaperFlipbookComponent>();
	AnimatedRenderComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	AnimatedRenderComponent->SetFlipbook(FlipbookDataBeingEdited.Get()->TargetFlipbook);
	AnimatedRenderComponent->UpdateBounds();
	AnimatedRenderComponent->Stop();
	
	PreviewScene->AddComponent(AnimatedRenderComponent.Get(), FTransform::Identity);
	
	bShowPivot = true;

	//DrawHelper.bDrawGrid = GetDefault<UFlipbookDataEditorSettings>()->bShowGridByDefault;
	DrawHelper.bDrawGrid = 0;

	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetCompositeEditorPrimitives(true);

	SetViewModes(VMI_Lit, VMI_Lit);

	// keep direction in mind
	const ELevelViewportType NewViewportType = LVT_OrthoXZ;
	SetViewportType(NewViewportType);

	GeometryEditMode = MakeShareable(new FSpriteGeometryEditMode());

	// Geometry edit mode
	GeometryEditMode->SetModeTools(GetModeTools());
	GeometryEditMode->SetEditorContext(this);

	GeometryEditMode->SetGeometryColors(
		FLinearColor(1.f, 1.f, 1.f, 1.f), 
		FLinearColor(1.f, 0.f, 1.f, 1.f));
}

void FFlipbookDataEditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
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

void FFlipbookDataEditorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);
	
	GeometryEditMode->Render(View, Viewport, PDI);
	
	if (bShowPivot && AnimatedRenderComponent.IsValid())
	{
		FUnrealEdUtils::DrawWidget(View, PDI, AnimatedRenderComponent->GetComponentTransform().ToMatrixWithScale(), 0, 0, EAxisList::Screen, EWidgetMovementMode::WMM_Translate);
	}
}


void FFlipbookDataEditorViewportClient::Tick(float DeltaSeconds)
{
	GeometryEditMode->Tick(this, DeltaSeconds);

	CurrentKeyFrameData = FlipbookDataEditorPtr.Pin().Get()->CreateKeyFrameDataOnCurrentFrame();
	if (CurrentKeyFrameData != nullptr && CurrentKeyFrameData != LastKeyFrameData) 
	{
		  const int32 curFrame = FlipbookDataEditorPtr.Pin().Get()->GetCurrentFrame();
		  FSpriteGeometryCollection* GeometryCollectionInstruction = CurrentKeyFrameData->KeyFrameInstructions.IsValidIndex(curFrame) ? &CurrentKeyFrameData->KeyFrameInstructions[0].CollisionGeometry : nullptr;
		  int32* InsID = FlipbookDataEditorPtr.Pin().Get()->GetGeoPropTabBody()->GetButtonFrameID();
		  
	      if (CurrentKeyFrameData->KeyFrameInstructions.Num() != 0)
	      {
		      GeometryEditMode->SetKeyFrameInstructionsBeingEdited(&CurrentKeyFrameData->KeyFrameInstructions, InsID, /*bAllowCircles=*/ false, /*bAllowSubtractivePolygons=*/ false);
	      }else
	      {
		      UE_LOG(LogTemp, Warning, TEXT("No frame instruction collection set"));
	      }

	      LastKeyFrameData = CurrentKeyFrameData;
	}
	
	if (AnimatedRenderComponent.IsValid())
	{		
		UPaperFlipbook* Flipbook = FlipbookDataBeingEdited.Get()->TargetFlipbook;

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

		FEditorViewportClient::Tick(DeltaSeconds);
	}

	if (!GIntraFrameDebuggingGameThread)
	{
		OwnedPreviewScene.GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

void FFlipbookDataEditorViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	const FViewportClick Click(&View, this, Key, Event, HitX, HitY);
	const bool bIsCtrlKeyDown = Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);
	const bool bIsShiftKeyDown = Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
	const bool bIsAltKeyDown = Viewport->KeyState(EKeys::LeftAlt) || Viewport->KeyState(EKeys::RightAlt);
	bool bHandled = false;

	GeometryEditMode->HandleClick(this, HitProxy, Click);
}

bool FFlipbookDataEditorViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	bool bHandled = false;
	FInputEventState InputState(InViewport, Key, Event);
	
	GeometryEditMode->InputKey(this, InViewport, Key, Event);
	
	// Pass keys to standard controls, if we didn't consume input
	return (bHandled) ? true : FEditorViewportClient::InputKey(InViewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
}

FLinearColor FFlipbookDataEditorViewportClient::GetBackgroundColor() const
{
	return GetDefault<UFlipbookDataEditorSettings>()->BackgroundColor;
}

FVector2D FFlipbookDataEditorViewportClient::SelectedItemConvertWorldSpaceDeltaToLocalSpace(const FVector& WorldSpaceDelta) const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? FVector2D(0.f) : Sprite->ConvertWorldSpaceDeltaToTextureSpace(WorldSpaceDelta);
}

FVector2D FFlipbookDataEditorViewportClient::WorldSpaceToTextureSpace(const FVector& SourcePoint) const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? FVector2D(0.f) : Sprite->ConvertWorldSpaceToTextureSpace(SourcePoint);
}

FVector FFlipbookDataEditorViewportClient::TextureSpaceToWorldSpace(const FVector2D& SourcePoint) const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? FVector(0.f) : Sprite->ConvertTextureSpaceToWorldSpace(SourcePoint);
}

float FFlipbookDataEditorViewportClient::SelectedItemGetUnitsPerPixel() const
{
	UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
	return Sprite == nullptr ? 0.f : Sprite->GetUnrealUnitsPerPixel();
}

void FFlipbookDataEditorViewportClient::BeginTransaction(const FText& SessionName)
{
	if (ScopedTransaction == nullptr)
	{
		ScopedTransaction = new FScopedTransaction(SessionName);

		UPaperSprite* Sprite = GetSpriteOnCurrentFrame();
		Sprite->Modify();
	}
}

void FFlipbookDataEditorViewportClient::MarkTransactionAsDirty()
{
	bManipulationDirtiedSomething = true;
	Invalidate();
	//@TODO: Can add a call to Sprite->PostEditChange here if we want to update the baked sprite data during a drag operation
	// (maybe passing in Interactive - if so, the EndTransaction PostEditChange needs to be a ValueSet)
}

void FFlipbookDataEditorViewportClient::EndTransaction()
{
	bManipulationDirtiedSomething = false;

	if (ScopedTransaction != nullptr)
	{
		delete ScopedTransaction;
		ScopedTransaction = nullptr;
	}
}

void FFlipbookDataEditorViewportClient::InvalidateViewportAndHitProxies()
{
	Viewport->Invalidate();
}

void FFlipbookDataEditorViewportClient::RequestFocusOnSelection(bool bInstant)
{
	bDeferZoomToSprite = true;
	bDeferZoomToSpriteIsInstant = bInstant;
}

UPaperSprite* FFlipbookDataEditorViewportClient::GetSpriteOnCurrentFrame() const
{
	const int32 curFrame = FlipbookDataEditorPtr.Pin().Get()->GetCurrentFrame();
	return FlipbookDataBeingEdited.Get()->TargetFlipbook->GetSpriteAtFrame(curFrame);
}

FFlipbookDataKeyFrame* FFlipbookDataEditorViewportClient::GetKeyFrameDataOnCurrentFrame() const
{
	const int32 curFrame = FlipbookDataEditorPtr.Pin().Get()->GetCurrentFrame();
	
	if (FlipbookDataBeingEdited.Get()->KeyFrameArray.IsValidIndex(curFrame))
	{
		return &FlipbookDataBeingEdited.Get()->KeyFrameArray[curFrame];
	}
	
	return nullptr;
}

UPaperFlipbookComponent * FFlipbookDataEditorViewportClient::GetPreviewComponent() const
{
  return AnimatedRenderComponent.Get();
}

void FFlipbookDataEditorViewportClient::BindCommands()
{	
	const FFlipbookDataGeometryEditCommands& Commands = FFlipbookDataGeometryEditCommands::Get();

	const TSharedRef<FSpriteGeometryEditMode> GeometryEditRef = GeometryEditMode.ToSharedRef();
	
	FSpriteGeometryEditingHelper* GeometryHelper = GeometryEditMode->GetGeometryHelper();
	return;
}

FBox FFlipbookDataEditorViewportClient::GetDesiredFocusBounds() const
{
	return AnimatedRenderComponent->Bounds.GetBox();
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
