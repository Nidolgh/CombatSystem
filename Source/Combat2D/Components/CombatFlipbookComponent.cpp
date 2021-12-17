// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatFlipbookComponent.h"
#include "CombatFlipbook.h"
#include "CombatFlipbookSceneProxy.h"
#include "Engine/CollisionProfile.h"

// Sets default values for this component's properties
UCombatFlipbookComponent::UCombatFlipbookComponent()
	: OwnerPaperFlipbookComp(nullptr)
	, PreviousFrameIndex(-1)
{
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	ActiveBodySetup = nullptr;
}

// Called when the game starts
void UCombatFlipbookComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCombatFlipbookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SetActiveBodySetupCombatFrame();
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatFlipbookComponent::SetActiveBodySetupCombatFrame()
{
	if (!IsValid(OwnerPaperFlipbookComp) && !IsValid(ActiveCombatFlipbook))
		return;
	
	const int32 CurFrameIndex = OwnerPaperFlipbookComp->GetPlaybackPositionInFrames();

	if (PreviousFrameIndex == CurFrameIndex)
		return;
	
	PreviousFrameIndex = CurFrameIndex;

	if (!ActiveCombatFlipbook->CombatFramesArray.IsValidIndex(CurFrameIndex))
		return;
	
	TArray<FCombatFrameCollisionData>& CombatFrameCollisionData = ActiveCombatFlipbook->CombatFramesArray[CurFrameIndex].CollisionDataArray;

	if (!CombatFrameCollisionData.IsValidIndex(0))
		return;
	
	ActiveBodySetup = CombatFrameCollisionData[0].GeneratedBodySetup; // TODO fix collision types

	CombatFlipbookSceneProxy->SetActiveBodySetup(ActiveBodySetup);

	RecreatePhysicsState();
	UpdateBounds();
}

FPrimitiveSceneProxy* UCombatFlipbookComponent::CreateSceneProxy()
{
	CombatFlipbookSceneProxy = new FCombatFlipbookSceneProxy(this);
	return CombatFlipbookSceneProxy;
}

FBoxSphereBounds UCombatFlipbookComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	if (ActiveBodySetup != nullptr)
	{
		FBoxSphereBounds NewBounds;

		const FBox AggGeomBox = ActiveBodySetup->AggGeom.CalcAABB(LocalToWorld);
		if (AggGeomBox.IsValid)
		{
			NewBounds = Union(NewBounds,FBoxSphereBounds(AggGeomBox));
		}

		// Apply bounds scale
		NewBounds.BoxExtent *= BoundsScale;
		NewBounds.SphereRadius *= BoundsScale;

		return NewBounds;
	}
	
	return FBoxSphereBounds(LocalToWorld.GetLocation(), FVector::ZeroVector, 0.f);
}

UBodySetup* UCombatFlipbookComponent::GetBodySetup()
{
	return ActiveBodySetup == nullptr ? Super::GetBodySetup() : ActiveBodySetup;
}

FLinearColor UCombatFlipbookComponent::GetWireframeColor() const
{
	if (Mobility == EComponentMobility::Static)
	{
		return FColor(0, 255, 255, 255);
	}
	else
	{
		if (BodyInstance.bSimulatePhysics)
		{
			return FColor(0, 255, 128, 255);
		}
		else
		{
			return FColor(255, 0, 255, 255);
		}
	}
}

void UCombatFlipbookComponent::SetActiveCombatFlipbook(UCombatFlipbook* NewActiveCombatFlipbook)
{
	RecreatePhysicsState();
	UpdateBounds();

	ActiveCombatFlipbook = NewActiveCombatFlipbook;
}

void UCombatFlipbookComponent::SetPaperFlipbookComponent(UPaperFlipbookComponent* NewPaperFlipbookComp)
{
	OwnerPaperFlipbookComp = NewPaperFlipbookComp;
	PreviousFrameIndex = NewPaperFlipbookComp->GetPlaybackPositionInFrames();
}

