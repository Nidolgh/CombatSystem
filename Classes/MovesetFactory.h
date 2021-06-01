#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MovesetFactory.generated.h"

UCLASS()
class COMBAT2DEDITOR_API UMovesetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UMovesetFactory();

	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface
};