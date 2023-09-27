#pragma once

#include "CombatSystemCollisionShapeInfo.generated.h"

UENUM(BlueprintType)
enum class ECombatSystemCollisionShapeType : uint8 {
	Box,
	Sphere,
	Capsule
};

USTRUCT(BlueprintType)
struct FCombatSystemCollisionShapeInfo
{
GENERATED_BODY()
	
	FCombatSystemCollisionShapeInfo()
	{
		UniqueObjID = InternalObjCount;
		InternalObjCount++;
	}

	bool operator==(const FCombatSystemCollisionShapeInfo& Other) const
	{
		return PhantomSceneComponent == Other.PhantomSceneComponent && ShapeType == Other.ShapeType && UniqueObjID == Other.UniqueObjID;
	}

	uint32 GetUniqueObjID() const
	{
		return UniqueObjID;
	}

	FName GetUniqueTag() const
	{
		const FString TagToAdd = FString::Printf(TEXT("CombatSystemShape_%i"), GetUniqueObjID());
		return FName(*TagToAdd);
	}

	UPROPERTY(VisibleAnywhere)
	FTransform Transform;

	UPROPERTY(VisibleAnywhere)
	ECombatSystemCollisionShapeType ShapeType;

	// DEBUG, used when editing shape transform in animation editor
	UPROPERTY(VisibleAnywhere)
	USceneComponent* PhantomSceneComponent = nullptr;

private:
	inline static uint32 InternalObjCount = 0;
	uint32 UniqueObjID = 0;
};
