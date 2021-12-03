// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Paper2D/Private/PaperRenderSceneProxy.h"

class COMBAT2D_API FCombatFlipbookSceneProxy : public FPaperRenderSceneProxy
{
public:
	FCombatFlipbookSceneProxy(class UCombatFlipbookComponent* InComponent);

	// FPrimitiveSceneProxy interface 
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
	// End of FPrimitiveSceneProxy interface 

protected:
	bool bDrawDebugCollision;
};
