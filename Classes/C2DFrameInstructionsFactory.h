#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "C2DFrameInstructionsFactory.generated.h"

UCLASS()
class COMBAT2DEDITOR_API UC2DFrameInstructionsFactory : public UFactory
{
	GENERATED_BODY()

public:
	UC2DFrameInstructionsFactory();

	// Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface
};