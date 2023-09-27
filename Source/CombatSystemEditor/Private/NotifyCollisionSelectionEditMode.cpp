#include "NotifyCollisionSelectionEditMode.h"

#include "IPersonaPreviewScene.h"
#include "AssetEditorModeManager.h"
#include "EngineUtils.h"
#include "Components/ShapeComponent.h"

#define LOCTEXT_NAMESPACE "NotifyCollisionSelectionEditMode"

FNotifyCollisionSelectionEditMode::FNotifyCollisionSelectionEditMode()
	: bManipulating(false)
	, bInTransaction(false)
	, SelectedShapeComponent(nullptr)
{
	bDrawGrid = false;
}

IPersonaPreviewScene& FNotifyCollisionSelectionEditMode::GetAnimPreviewScene() const
{
	return *static_cast<IPersonaPreviewScene*>(static_cast<FAssetEditorModeManager*>(Owner)->GetPreviewScene());
}

bool FNotifyCollisionSelectionEditMode::GetCameraTarget(FSphere& OutTarget) const
{
	// Center rotation point, camera target
	bool bHandled = false;

	if (IsValid(SelectedShapeComponent))
	{
		bHandled = true;

		OutTarget.Center = SelectedShapeComponent->GetComponentLocation();
		OutTarget.W = 30.0f;
	}
	
	return bHandled;
}

bool FNotifyCollisionSelectionEditMode::StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	if (IsValid(SelectedShapeComponent))
	{
		const UE::Widget::EWidgetMode WidgetMode = InViewportClient->GetWidgetMode();

		bManipulating = true;

		if (!bInTransaction)
		{
			if (WidgetMode == UE::Widget::WM_Scale)
			{
				GEditor->BeginTransaction(LOCTEXT("NotifyCollisionSelection_ScaleComponent", "Scale Component"));
			}
			else if (WidgetMode == UE::Widget::WM_Rotate)
			{
				GEditor->BeginTransaction(LOCTEXT("NotifyCollisionSelection_RotateComponent", "Rotate Component"));
			}
			else
			{
				GEditor->BeginTransaction(LOCTEXT("NotifyCollisionSelection_TranslateComponent", "Translate Component"));
			}

			SelectedShapeComponent->SetFlags(RF_Transactional);
			SelectedShapeComponent->Modify();
			
			bInTransaction = true;
		}
		
		return true;
	}

	return false;
}

bool FNotifyCollisionSelectionEditMode::EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	if (bManipulating)
	{
		if (bInTransaction)
		{
			GEditor->EndTransaction();
			bInTransaction = false;
		}
		
		bManipulating = false;
		return true;
	}
	
	return false;
}

bool FNotifyCollisionSelectionEditMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport,
	FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	const EAxisList::Type CurrentAxis = InViewportClient->GetCurrentWidgetAxis();
	const UE::Widget::EWidgetMode WidgetMode = InViewportClient->GetWidgetMode();
	
	bool bHandled = false;

	if (bManipulating && CurrentAxis != EAxisList::None)
	{
		bHandled = true;

		if (WidgetMode == UE::Widget::WM_Scale)
		{
			FVector Scale = SelectedShapeComponent->GetComponentScale();
			Scale += InScale;

			SelectedShapeComponent->SetWorldScale3D(Scale);
		}
		if (WidgetMode == UE::Widget::WM_Rotate)
		{
			const FTransform Transform = SelectedShapeComponent->GetComponentTransform();
			const FRotator NewRotation = (Transform * FTransform(InRot)).Rotator();

			SelectedShapeComponent->SetWorldRotation(NewRotation);
		}
		else
		{
			FVector Location = SelectedShapeComponent->GetComponentLocation();
			Location += InDrag;
			SelectedShapeComponent->SetWorldLocation(Location);
		}

		InViewport->Invalidate();
	}
	
	return bHandled;
}

FVector FNotifyCollisionSelectionEditMode::GetWidgetLocation() const
{
	if (IsValid(SelectedShapeComponent))
	{
		return SelectedShapeComponent->GetComponentLocation();
	}
	
	return FVector::ZeroVector;
}

bool FNotifyCollisionSelectionEditMode::ShouldDrawWidget() const
{
	if (IsValid(SelectedShapeComponent))
	{
		return true;
	}
	
	return false;
}

bool FNotifyCollisionSelectionEditMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key,
                                                 EInputEvent Event)
{
	if (Key == EKeys::Delete && IsValid(SelectedShapeComponent))
	{
		SelectedShapeComponent->Deactivate();
		// We want to trigger the TransformUpdated lambda bound in the AnimNotify to catch and remove this component
		SelectedShapeComponent->AddRelativeLocation(FVector::One());
		SelectedShapeComponent = nullptr;
	}
	
	return IPersonaEditMode::InputKey(ViewportClient, Viewport, Key, Event);
}

bool FNotifyCollisionSelectionEditMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy,
                                                    const FViewportClick& Click)
{
	if (!HitProxy)
	{
		SelectedShapeComponent = nullptr;

		return false;
	}
	
	if (const HActor* ActorHitProxy = HitProxyCast<HActor>(HitProxy))
	{
		if (ActorHitProxy->PrimComponent)
		{
			const UShapeComponent* PrimAsShapeComp = Cast<UShapeComponent>(ActorHitProxy->PrimComponent);
			if (ActorHitProxy->PrimComponent->IsA<UShapeComponent>())
			{
				if (PrimAsShapeComp->bSelectable)
				{
					// TODO: Find HitProxy that does not have selected component as const!
					SelectedShapeComponent = const_cast<UShapeComponent*>(PrimAsShapeComp);
					
					return true;
				}
			}
		}
	}

	SelectedShapeComponent = nullptr;
	
	return IPersonaEditMode::HandleClick(InViewportClient, HitProxy, Click);
}