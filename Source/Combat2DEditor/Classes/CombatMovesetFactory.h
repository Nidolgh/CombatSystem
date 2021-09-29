#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CombatMovesetFactory.generated.h"

UCLASS()
class COMBAT2DEDITOR_API UCombatMovesetFactory : public UFactory
{
	GENERATED_BODY()

	UCombatMovesetFactory();
	
	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface
};