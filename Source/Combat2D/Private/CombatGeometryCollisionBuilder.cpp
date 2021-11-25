#include "CombatGeometryCollisionBuilder.h"

//////////////////////////////////////////////////////////////////////////
// FCombatGeometryCollisionBuilder

// from PaperSprite.cpp
FCombatGeometryCollisionBuilder::FCombatGeometryCollisionBuilder(UBodySetup* InBodySetup)
	: MyBodySetup(InBodySetup)
	, UnrealUnitsPerPixel(1.0f)
	, CollisionThickness(64.0f)
	, ZOffsetAmount(0.0f)
	, CollisionDomain(ESpriteCollisionMode::Use3DPhysics)
{
	check(MyBodySetup);

	PaperAxisX = FVector(1.0f, 0.0f, 0.0f);
	PaperAxisY = FVector(0.0f, 0.0f, 1.0f);
	PaperAxisZ = FVector::CrossProduct(PaperAxisX, PaperAxisY);
}

void FCombatGeometryCollisionBuilder::ProcessGeometry(const FSpriteGeometryCollection& InGeometry)
{
	// Add geometry to the body setup
	AddBoxCollisionShapesToBodySetup(InGeometry);
	AddPolygonCollisionShapesToBodySetup(InGeometry);
	AddCircleCollisionShapesToBodySetup(InGeometry);
}

void FCombatGeometryCollisionBuilder::Finalize()
{
	// Rebuild the body setup
	MyBodySetup->InvalidatePhysicsData();
	MyBodySetup->CreatePhysicsMeshes();
}

void FCombatGeometryCollisionBuilder::AddBoxCollisionShapesToBodySetup(const FSpriteGeometryCollection& InGeometry)
{
	// Bake all of the boxes to the body setup
	for (const FSpriteGeometryShape& Shape : InGeometry.Shapes)
	{
		if (Shape.ShapeType == ESpriteShapeType::Box)
		{
			// Determine the box size and center in pivot space
			const FVector2D& BoxSizeInTextureSpace = Shape.BoxSize;
			const FVector2D CenterInTextureSpace = Shape.BoxPosition;
			const FVector2D CenterInPivotSpace = ConvertTextureSpaceToPivotSpace(CenterInTextureSpace);

			// Convert from pixels to uu
			const FVector2D BoxSizeInPivotSpace = ConvertTextureSpaceToPivotSpaceNoTranslation(BoxSizeInTextureSpace);
			const FVector2D BoxSize2D = BoxSizeInPivotSpace * UnrealUnitsPerPixel;
			const FVector2D CenterInScaledSpace = CenterInPivotSpace * UnrealUnitsPerPixel;

			// Create a new box primitive
			switch (CollisionDomain)
			{
				case ESpriteCollisionMode::Use3DPhysics:
					{
						const FVector BoxPos3D = (PaperAxisX * CenterInScaledSpace.X) + (PaperAxisY * CenterInScaledSpace.Y) + (PaperAxisZ * ZOffsetAmount);
						const FVector BoxSize3D = (PaperAxisX * BoxSize2D.X) + (PaperAxisY * BoxSize2D.Y) + (PaperAxisZ * CollisionThickness);

						// Create a new box primitive
						FKBoxElem& Box = *new (MyBodySetup->AggGeom.BoxElems) FKBoxElem(FMath::Abs(BoxSize3D.X), FMath::Abs(BoxSize3D.Y), FMath::Abs(BoxSize3D.Z));
						Box.Center = BoxPos3D;
						Box.Rotation = FRotator(Shape.Rotation, 0.0f, 0.0f);
					}
					break;
				default:
					check(false);
					break;
			}
		}
	}
}

void FCombatGeometryCollisionBuilder::AddPolygonCollisionShapesToBodySetup(const FSpriteGeometryCollection& InGeometry)
{
	// Rebuild the runtime geometry for polygons
	TArray<FVector2D> CollisionData;
	InGeometry.Triangulate(/*out*/ CollisionData, /*bIncludeBoxes=*/ false);

	// Adjust the collision data to be relative to the pivot and scaled from pixels to uu
	for (FVector2D& Point : CollisionData)
	{
		Point = ConvertTextureSpaceToPivotSpace(Point) * UnrealUnitsPerPixel;
	}

	//@TODO: Use this guy instead: DecomposeMeshToHulls
	//@TODO: Merge triangles that are convex together!

	// Bake it to the runtime structure
	switch (CollisionDomain)
	{
	case ESpriteCollisionMode::Use3DPhysics:
		{
			UBodySetup* BodySetup3D = MyBodySetup;

			const FVector HalfThicknessVector = PaperAxisZ * 0.5f * CollisionThickness;

			int32 RunningIndex = 0;
			for (int32 TriIndex = 0; TriIndex < CollisionData.Num() / 3; ++TriIndex)
			{
				FKConvexElem& ConvexTri = *new (BodySetup3D->AggGeom.ConvexElems) FKConvexElem();
				ConvexTri.VertexData.Empty(6);
				for (int32 Index = 0; Index < 3; ++Index)
				{
					const FVector2D& Pos2D = CollisionData[RunningIndex++];
					
					const FVector Pos3D = (PaperAxisX * Pos2D.X) + (PaperAxisY * Pos2D.Y) + (PaperAxisZ * ZOffsetAmount);

					new (ConvexTri.VertexData) FVector(Pos3D - HalfThicknessVector);
					new (ConvexTri.VertexData) FVector(Pos3D + HalfThicknessVector);
				}
				ConvexTri.UpdateElemBox();
			}
		}
		break;
	default:
		check(false);
		break;
	}
}

void FCombatGeometryCollisionBuilder::AddCircleCollisionShapesToBodySetup(const FSpriteGeometryCollection& InGeometry)
{
	// Bake all of the boxes to the body setup
	for (const FSpriteGeometryShape& Shape : InGeometry.Shapes)
	{
		if (Shape.ShapeType == ESpriteShapeType::Circle)
		{
			// Determine the box size and center in pivot space
			const FVector2D& CircleSizeInTextureSpace = Shape.BoxSize;
			const FVector2D& CenterInTextureSpace = Shape.BoxPosition;
			const FVector2D CenterInPivotSpace = ConvertTextureSpaceToPivotSpace(CenterInTextureSpace);

			// Convert from pixels to uu
			const FVector2D CircleSizeInPivotSpace = ConvertTextureSpaceToPivotSpaceNoTranslation(CircleSizeInTextureSpace);
			const FVector2D CircleSize2D = CircleSizeInPivotSpace * UnrealUnitsPerPixel;
			const FVector2D CenterInScaledSpace = CenterInPivotSpace * UnrealUnitsPerPixel;

			//@TODO: Neither Box2D nor PhysX support ellipses, currently forcing to be circular, but should we instead convert to an n-gon?
			const float AverageDiameter = (FMath::Abs(CircleSize2D.X) + FMath::Abs(CircleSize2D.Y)) * 0.5f;
			const float AverageRadius = AverageDiameter * 0.5f;

			// Create a new circle/sphere primitive
			switch (CollisionDomain)
			{
				case ESpriteCollisionMode::Use3DPhysics:
					{
						// Create a new box primitive
						FKSphereElem& Sphere = *new (MyBodySetup->AggGeom.SphereElems) FKSphereElem(AverageRadius);
						Sphere.Center = (PaperAxisX * CenterInScaledSpace.X) + (PaperAxisY * CenterInScaledSpace.Y) + (PaperAxisZ * ZOffsetAmount);
					}
					break;
				default:
					check(false);
					break;
			}
		}
	}
}

FVector2D FCombatGeometryCollisionBuilder::ConvertTextureSpaceToPivotSpace(const FVector2D& Input) const
{
	return Input;
}

FVector2D FCombatGeometryCollisionBuilder::ConvertTextureSpaceToPivotSpaceNoTranslation(const FVector2D& Input) const
{
	return Input;
}
