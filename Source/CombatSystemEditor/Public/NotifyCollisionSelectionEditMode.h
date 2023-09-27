#pragma once

#include "CoreMinimal.h"
#include "IPersonaEditMode.h"

/**
 * Based on SkeletonSelectionEditMode
 */
class FNotifyCollisionSelectionEditMode : public IPersonaEditMode
{
public:
	FNotifyCollisionSelectionEditMode();
	
	/** IPersonaEditMode interface */
	virtual IPersonaPreviewScene& GetAnimPreviewScene() const override;
	virtual bool GetCameraTarget(FSphere& OutTarget) const override;

	/** FEdMode interface */
	virtual bool StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	virtual bool EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual FVector GetWidgetLocation() const override;
	virtual bool ShouldDrawWidget() const override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	
private:
	UShapeComponent* SelectedShapeComponent;
	void Test();
	/** Whether we are currently in a manipulation  */
	bool bManipulating;	

	/** Whether we are currently in a transaction  */
	bool bInTransaction;
};
