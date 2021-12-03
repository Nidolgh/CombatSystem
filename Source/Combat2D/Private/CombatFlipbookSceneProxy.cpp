// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatFlipbookSceneProxy.h"

#include "../components/CombatFlipbookComponent.h"
#include "Materials/Material.h"
#include "SceneManagement.h"
#include "PhysicsEngine/BodySetup.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "RHIResources.h"

FCombatFlipbookSceneProxy::FCombatFlipbookSceneProxy(UCombatFlipbookComponent* InComponent)
	: FPaperRenderSceneProxy(InComponent)
	, bDrawDebugCollision(true)
{
	SetWireframeColor(InComponent->GetWireframeColor());
}

void FCombatFlipbookSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
	const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	const FEngineShowFlags& EngineShowFlags = ViewFamily.EngineShowFlags;
	
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			if (bDrawDebugCollision)
			{
				const FSceneView* View = Views[ViewIndex];
				DebugDrawCollision(View, ViewIndex, Collector, false);
			}

			// Draw bounds
			#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if (EngineShowFlags.Paper2DSprites)
			{
				RenderBounds(Collector.GetPDI(ViewIndex), EngineShowFlags, GetBounds(), (Owner == nullptr) || IsSelected());
			}
#endif
		}
	}
}
