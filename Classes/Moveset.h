#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "FlipbookData.h"

#include "Moveset.generated.h"

// https://answers.unrealengine.com/questions/25049/what-does-coreuobject-api-macro.html

UCLASS()
class COMBAT2DEDITOR_API UMoveset : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Moveset)
		FString Description;

	UPROPERTY(EditAnywhere, Category = Moveset)
		TArray<UFlipbookData*> FlipbookDataArray;
};