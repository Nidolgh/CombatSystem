// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"

class COMBAT2D_API FCombatFlipbookSceneProxy : public FPrimitiveSceneProxy
{
public:
	FCombatFlipbookSceneProxy(class UCombatFlipbookComponent* InComponent);

	virtual SIZE_T GetTypeHash() const override;

	// FPrimitiveSceneProxy interface
	virtual uint32 GetMemoryFootprint() const override;
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	// End of FPrimitiveSceneProxy interface

protected:
	const UBodySetup* BodySetup;
};
