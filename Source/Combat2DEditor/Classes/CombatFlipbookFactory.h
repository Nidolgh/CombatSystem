#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CombatFlipbookFactory.generated.h"

UCLASS()
class COMBAT2DEDITOR_API UCombatFlipbookFactory : public UFactory
{
	GENERATED_BODY()

	UCombatFlipbookFactory();
	
	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface
};