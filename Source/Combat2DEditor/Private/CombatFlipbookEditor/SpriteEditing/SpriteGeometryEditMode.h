// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "EdMode.h"
#include "SpriteGeometryEditing.h"

#include "CombatFlipbook.h"

class FCanvas;
class FEditorViewportClient;
class FPrimitiveDrawInterface;
class FSceneView;
class FUICommandList;
class FViewport;
struct FSpriteGeometryCollection;
struct FViewportClick;

//////////////////////////////////////////////////////////////////////////
// FSpriteGeometryEditMode

class FSpriteGeometryEditMode : public FEdMode
{
public:
	static const FEditorModeID EM_SpriteGeometry;
	static const FLinearColor MarqueeDrawColor;
	
public:
	FSpriteGeometryEditMode();

	// FEdMode interface
	virtual void Initialize() override;
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual FVector GetWidgetLocation() const override;
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool ShouldDrawWidget() const override;
	// End of FEdMode interface

	// Changes the editor interface to point to the hosting editor; this is basically required
	void SetEditorContext(class ISpriteSelectionContext* InNewEditorContext);

	// Sets the default bounds for newly created boxes/circles/etc...
	void SetNewGeometryPreferredBounds(FBox2D& NewDesiredBounds);

	// Sets the draw color for geometry
	void SetGeometryColors(const FLinearColor& NewVertexColor, const FLinearColor& NewNegativeVertexColor);

	// Changes the geometry being edited (clears the selection set in the process)
	void SetKeyFrameInstructionsBeingEdited(TArray<FCombatFrameCollisionData>* NewKeyFrameInstructions, int32* NewInstructionToEdit, bool bInAllowCircles, bool bInAllowSubtractivePolygons);
	
	void BindCommands(TSharedPtr<FUICommandList> InCommandList);

	FVector2D GetMarqueeStartPos() const { return MarqueeStartPos; }
	FVector2D GetMarqueeEndPos() const { return MarqueeEndPos; }
	bool ProcessMarquee(FViewport* Viewport, FKey Key, EInputEvent Event, bool bMarqueeStartModifierPressed);

	static void DrawGeometryStats(FViewport& InViewport, FSceneView& View, FCanvas& Canvas, const FSpriteGeometryCollection& Geometry, bool bIsRenderGeometry, int32& YPos);
	static void DrawCollisionStats(FViewport& InViewport, FSceneView& View, FCanvas& Canvas, class UBodySetup* BodySetup, int32& YPos);

	FSpriteGeometryEditingHelper* GetGeometryHelper() { return &SpriteGeometryHelper; }

	void AddBoxShape();
	void AddCircleShape();

	void SetModeTools(FEditorModeTools* ModeTools);

protected:
	FBox2D BoundsForNewShapes;
	FLinearColor GeometryVertexColorActive;
	FLinearColor GeometryVertexColor;
	FLinearColor GeometryVertexColorHitbox;
	FLinearColor GeometryVertexColorHurtbox;
	
	FLinearColor NegativeGeometryVertexColor;

	// Sprite geometry editing/rendering helper
	FSpriteGeometryEditingHelper SpriteGeometryHelper;

	TArray<FCombatFrameCollisionData>* KeyFrameInstructions;
	int32* InstructionToEdit;
	
	// Marquee tracking
	bool bIsMarqueeTracking;
	FVector2D MarqueeStartPos;
	FVector2D MarqueeEndPos;

protected:
	void SetActiveGeometryColor(const ECollisionType InsType, const bool IsActive);
	void SetEditingGeometry();

	bool IsEditingGeometry() const;

	void SelectVerticesInMarquee(FEditorViewportClient* ViewportClient, FViewport* Viewport, bool bAddToSelection);

	void DrawMarquee(FViewport& InViewport, const FSceneView& View, FCanvas& Canvas, const FLinearColor& Color);
};
