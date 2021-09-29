#pragma once

#include "CoreMinimal.h"

#include "CombatFlipbook.h"

#include "CombatMoveset.generated.h"

UCLASS()
class COMBAT2D_API UCombatMoveset : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Moveset)
		FString Description;

	UPROPERTY(EditAnywhere, Category = Moveset)
		TArray<UCombatFlipbook*> CombatFlipbookArray;
};