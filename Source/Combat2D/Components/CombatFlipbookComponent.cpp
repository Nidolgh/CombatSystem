// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatFlipbookComponent.h"
#include "CombatFlipbook.h"
#include "CombatFlipbookSceneProxy.h"
#include "CombatGeometryCollisionBuilder.h"
#include "SpriteDrawCall.h"
#include "Engine/CollisionProfile.h"

// Sets default values for this component's properties
UCombatFlipbookComponent::UCombatFlipbookComponent()
{
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	BodySetup = nullptr;
}

// Called when the game starts
void UCombatFlipbookComponent::BeginPlay()
{
	Super::BeginPlay();
	
	RebuildCollision();
}


// Called every frame
void UCombatFlipbookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FPrimitiveSceneProxy* UCombatFlipbookComponent::CreateSceneProxy()
{
	FCombatFlipbookSceneProxy* NewProxy = new FCombatFlipbookSceneProxy(this);
	
	return NewProxy;
}

FBoxSphereBounds UCombatFlipbookComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	if (BodySetup != nullptr)
	{
		FBoxSphereBounds NewBounds;

		const FBox AggGeomBox = BodySetup->AggGeom.CalcAABB(LocalToWorld);
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
	return BodySetup == nullptr ? Super::GetBodySetup() : BodySetup;
}

void UCombatFlipbookComponent::RebuildCollision()
{
	UBodySetup* OldBodySetup = BodySetup;

	if (ActiveCombatFlipbook == nullptr)
		return;
	
	FCombatFrameCollisionData& CollisionData = ActiveCombatFlipbook->CombatFramesArray[0].CollisionDataArray[0];
	FSpriteGeometryCollection& CollisionGeometry = CollisionData.CollisionGeometry;
	
	// Ensure we have the data structure for the desired collision method
	switch (CollisionData.SpriteCollisionDomain)
	{
	case ESpriteCollisionMode::Use3DPhysics:
		BodySetup = NewObject<UBodySetup>(this);
		break;
	case ESpriteCollisionMode::None:
	default:
		BodySetup = nullptr;
		CollisionGeometry.Reset();
		break;
	}
	
	if (CollisionData.SpriteCollisionDomain != ESpriteCollisionMode::None)
	{
		check(BodySetup);
		BodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
	
		// Clean up the geometry (converting polygons back to bounding boxes, etc...)
		CollisionGeometry.ConditionGeometry();
	
		// Take the geometry and add it to the body setup
		FCombatGeometryCollisionBuilder CollisionBuilder(BodySetup);
		CollisionBuilder.ProcessGeometry(CollisionGeometry);
		CollisionBuilder.Finalize();
		
		RecreatePhysicsState();
    	UpdateBounds();
		

		// Copy across or initialize the only editable property we expose on the body setup
		if (OldBodySetup != nullptr)
		{
			BodySetup->DefaultInstance.CopyBodyInstancePropertiesFrom(&(OldBodySetup->DefaultInstance));
		}
		else
		{
			BodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
		}
	}
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

