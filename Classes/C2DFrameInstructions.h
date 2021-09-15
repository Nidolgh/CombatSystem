#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "SpriteEditorOnlyTypes.h"

#include "PaperFlipbook.h"
#include "C2DFrameInstructions.generated.h"

// https://answers.unrealengine.com/questions/25049/what-does-coreuobject-api-macro.html

UENUM()
enum class InstructionType
{
	HitBox = 0,
	HurtBox,
	InstructionTypeCount
};

USTRUCT()
struct FC2DFrameInstructionsKeyFrameInstruction
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
struct FC2DFrameInstructionsKeyFrame
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = C2DFrameInstructions)
		TArray<FC2DFrameInstructionsKeyFrameInstruction> KeyFrameInstructions;
};

UCLASS()
class COMBAT2DEDITOR_API UC2DFrameInstructions : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = C2DFrameInstructions)
	UPaperFlipbook* TargetFlipbook;

	UPROPERTY(EditAnywhere, Category = C2DFrameInstructions)
		FString Description;
	
	UPROPERTY(EditAnywhere, Category = C2DFrameInstructions)
	TArray<FC2DFrameInstructionsKeyFrame> KeyFrameArray;
};