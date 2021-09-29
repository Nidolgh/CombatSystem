#pragma once

#include "CoreMinimal.h"

#include "SpriteEditorOnlyTypes.h"
#include "PaperFlipbook.h"

#include "CombatFlipbook.generated.h"

UENUM()
enum class ECollisionType
{
	HitBox = 0,
	HurtBox,
	Count
};

USTRUCT()
struct FCombatFrameCollisionData
{
	GENERATED_USTRUCT_BODY()

	// Collision domain (no collision, 2D, or 3D)
	UPROPERTY(EditAnywhere, Category=Combat2DFlipbook)
		TEnumAsByte<ESpriteCollisionMode::Type> SpriteCollisionDomain;

	// Custom collision geometry polygons (in texture space)
	UPROPERTY(EditAnywhere, Category=Combat2DFlipbook)
		FSpriteGeometryCollection CollisionGeometry;

	// The extrusion thickness of collision geometry when using a 3D collision domain
	UPROPERTY(EditAnywhere, Category=Combat2DFlipbook)
		float CollisionThickness;

	// The color the Collisions object will render with
	UPROPERTY(EditAnywhere, Category=Combat2DFlipbook)
		FColor CollisionColor;

	// The color the Collisions object will render with
	UPROPERTY(EditAnywhere, Category=Combat2DFlipbook)
		ECollisionType CollisionType;
};

USTRUCT()
struct FCombatFrames
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Combat2DFlipbook)
	TArray<FCombatFrameCollisionData> CollisionDataArray;
};

UCLASS()
class COMBAT2D_API UCombatFlipbook : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat2DFlipbook)
		UPaperFlipbook* TargetFlipbook;

	UPROPERTY(EditAnywhere, Category = Combat2DFlipbook)
		FString Description;

	UPROPERTY(EditAnywhere, Category = Combat2DFlipbook)
		TArray<FCombatFrames> CombatFramesArray;
};