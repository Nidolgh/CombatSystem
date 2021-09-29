#include "CombatFlipbookFactory.h"
#include "CombatFlipbook.h"

UCombatFlipbookFactory::UCombatFlipbookFactory()
{
	// Provide the factory with information about how to handle our asset
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCombatFlipbook::StaticClass();
}

UObject* UCombatFlipbookFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Create and return a new instance of our MyCustomAsset object
	UCombatFlipbook* CombatFlipbookAsset = NewObject<UCombatFlipbook>(InParent, Class, Name, Flags);
	return CombatFlipbookAsset;
}