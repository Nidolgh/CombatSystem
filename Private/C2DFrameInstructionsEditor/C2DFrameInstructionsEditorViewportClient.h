// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "PreviewScene.h"
#include "AssetData.h"
#include "C2DFrameInstructionsEditor.h"
#include "SEditorViewport.h"
#include "C2DFrameInstructionsEditorViewportClient.h"

#include "SpriteGeometryEditMode.h"

#include "SpriteEditorSelections.h"
#include "Combat2DEditor/Classes/C2DFrameInstructions.h"

class UPaperFlipbookComponent;
class FCanvas;

struct FViewportSelectionRectangle
{
	FVector2D TopLeft;
	FVector2D Dimensions;
	FLinearColor Color;
};

//////////////////////////////////////////////////////////////////////////
// FSpriteEditorViewportClient

class FC2DFrameInstructionsEditorViewportClient : public FEditorViewportClient, public ISpriteSelectionContext
{
public:
	/** Constructor */
	FC2DFrameInstructionsEditorViewportClient(TWeakPtr<FC2DFrameInstructionsEditor> InC2DFrameInstructionsEditor, TWeakPtr<class SEditorViewport> InC2DFrameInstructionsEditorViewportPtr);

	// FViewportClient interface
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	virtual void Tick(float DeltaSeconds) override;
	// End of FViewportClient interface

	// FEditorViewportClient interface
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad) override;
	virtual FLinearColor GetBackgroundColor() const override;
	// End of FEditorViewportClient interface
	// 
	// ISpriteSelectionContext interface
	virtual FVector2D SelectedItemConvertWorldSpaceDeltaToLocalSpace(const FVector& WorldSpaceDelta) const override;
	virtual FVector2D WorldSpaceToTextureSpace(const FVector& SourcePoint) const override;
	virtual FVector TextureSpaceToWorldSpace(const FVector2D& SourcePoint) const override;
	virtual float SelectedItemGetUnitsPerPixel() const override;
	virtual void BeginTransaction(const FText& SessionName) override;
	virtual void MarkTransactionAsDirty() override;
	virtual void EndTransaction() override;
	virtual void InvalidateViewportAndHitProxies() override;
	// End of ISpriteSelectionContext interface
	
	void ToggleShowPivot() { bShowPivot = !bShowPivot;/*
		EPropertyEditorGuidActions::Invalidate(); */}
	bool IsShowPivotChecked() const { return bShowPivot; }

	// Called to request a focus on the current selection
	virtual void RequestFocusOnSelection(bool bInstant);

	/** Modifies the checkerboard texture's data */
	void ModifyCheckerboardTextureColors();

	void SetZoomPos(FVector2D InNewPos, float InNewZoom)
	{
		ZoomPos = InNewPos;
		ZoomAmount = InNewZoom;
	}

	UPaperSprite* GetSpriteOnCurrentFrame() const;
	FC2DFrameInstructionsKeyFrame* GetKeyFrameDataOnCurrentFrame() const;
	
	UPaperFlipbookComponent* GetPreviewComponent() const;

	// List of selection rectangles to draw
	TArray<FViewportSelectionRectangle> SelectionRectangles;

	TSharedPtr<FSpriteGeometryEditMode> GeometryEditMode;
	
protected:
	/** Checkerboard texture */
	UTexture2D* CheckerboardTexture;
	FVector2D ZoomPos;
	float ZoomAmount;

	void BindCommands();
	FBox GetDesiredFocusBounds() const;
private:

	// The preview scene
	FPreviewScene OwnedPreviewScene;

	// Sprite editor that owns this viewport
	TWeakPtr<FC2DFrameInstructionsEditor> C2DFrameInstructionsEditorPtr;

	// Render component for the sprite being edited
	TWeakObjectPtr<UPaperFlipbookComponent> AnimatedRenderComponent;

	// Should we show the sprite pivot?
	bool bShowPivot;

	// Should we show sockets?
	bool bShowSockets;
	
	// Should we zoom to the focus bounds next tick?
	bool bDeferZoomToSprite;
	bool bDeferZoomToSpriteIsInstant;

	// The flipbook being displayed in this client
	TAttribute<class UC2DFrameInstructions*> C2DFrameInstructionsBeingEdited;

	// A cached pointer to the flipbook that was being edited last frame. Used for invalidation reasons.
	TWeakObjectPtr<class UPaperFlipbook> FlipbookBeingEditedLastFrame;


	// The current transaction for undo/redo
	class FScopedTransaction* ScopedTransaction = nullptr;

	// Did we dirty something during manipulation?
	bool bManipulationDirtiedSomething;

	FC2DFrameInstructionsKeyFrame* CurrentKeyFrameData = nullptr;
	FC2DFrameInstructionsKeyFrame* LastKeyFrameData = nullptr;
};
