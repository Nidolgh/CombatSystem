#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "C2DFrameInstructions.h"

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
		TArray<UC2DFrameInstructions*> C2DFrameInstructionsArray;
};