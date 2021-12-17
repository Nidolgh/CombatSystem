#pragma once
#include "CombatFlipbook.h"

// A helper class for converting one or more FSpriteGeometryCollection objects into a UBodySetup object (From PaperSprite)
struct FCombatGeometryCollisionBuilder
{
protected:
	class UBodySetup* MyBodySetup;
	float UnrealUnitsPerPixel;
	float CollisionThickness;
	float ZOffsetAmount;
	ESpriteCollisionMode::Type CollisionDomain;

public:
	FCombatGeometryCollisionBuilder(class UBodySetup* InBodySetup);

	virtual void ProcessGeometry(const FSpriteGeometryCollection& InGeometry);
	virtual void Finalize();

	void SetUnrealUnitsPerPixel(const float NewUnrealUnitsPerPixel);
	void SetCollisionThickness(const float NewCollisionThickness);
	
protected:
	void AddBoxCollisionShapesToBodySetup(const FSpriteGeometryCollection& InGeometry);
	void AddPolygonCollisionShapesToBodySetup(const FSpriteGeometryCollection& InGeometry);
	void AddCircleCollisionShapesToBodySetup(const FSpriteGeometryCollection& InGeometry);

	virtual FVector2D ConvertTextureSpaceToPivotSpace(const FVector2D& Input) const;
	virtual FVector2D ConvertTextureSpaceToPivotSpaceNoTranslation(const FVector2D& Input) const;

	FVector PaperAxisX;
	FVector PaperAxisY;
	FVector PaperAxisZ;
};