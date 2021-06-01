#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "FlipbookDataFactory.generated.h"

UCLASS()
class COMBAT2DEDITOR_API UFlipbookDataFactory : public UFactory
{
	GENERATED_BODY()

public:
	UFlipbookDataFactory();

	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface
};