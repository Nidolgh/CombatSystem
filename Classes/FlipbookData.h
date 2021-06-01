#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "SpriteEditorOnlyTypes.h"

#include "PaperFlipbook.h"
#include "FlipbookData.generated.h"

// https://answers.unrealengine.com/questions/25049/what-does-coreuobject-api-macro.html

UENUM()
enum class InstructionType
{
	HitBox = 0,
	HurtBox,
	InstructionTypeCount
};

USTRUCT()
struct FFlipbookDataKeyFrameInstruction
{
	GENERATED_USTRUCT_BODY()

	
	
	UPROPERTY(EditAnywhere, Category=FrameInstruction, meta=(ClampMin=1))
	float Damage;

	UPROPERTY(EditAnywhere, Category=FrameInstruction)
	float HitStun;
	
	// Collision domain (no collision, 2D, or 3D)
	UPROPERTY(EditAnywhere, Category=Collision)
		TEnumAsByte<ESpriteCollisionMode::Type> SpriteCollisionDomain;
	
	// Custom collision geometry polygons (in texture space)
	UPROPERTY(EditAnywhere, Category=Collision)
		FSpriteGeometryCollection CollisionGeometry;

	// The extrusion thickness of collision geometry when using a 3D collision domain
	UPROPERTY(EditAnywhere, Category=Collision)
		float CollisionThickness;

	// The color the Collisions object will render with
	UPROPERTY(EditAnywhere, Category=Collision)
		FColor CollisionColor;

	// The color the Collisions object will render with
	UPROPERTY(EditAnywhere, Category=Collision)
		InstructionType InstructionType;
};

USTRUCT()
struct FFlipbookDataKeyFrame
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = FlipbookData)
		TArray<FFlipbookDataKeyFrameInstruction> KeyFrameInstructions;
};

UCLASS()
class COMBAT2DEDITOR_API UFlipbookData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FlipbookData)
	UPaperFlipbook* TargetFlipbook;

	UPROPERTY(EditAnywhere, Category = FlipbookData)
		FString Description;
	
	UPROPERTY(EditAnywhere, Category = FlipbookData)
	TArray<FFlipbookDataKeyFrame> KeyFrameArray;
};