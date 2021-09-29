#include "CombatMovesetFactory.h"
#include "CombatMoveset.h"

UCombatMovesetFactory::UCombatMovesetFactory()
{
	// Provide the factory with information about how to handle our asset
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCombatMoveset::StaticClass();
}

UObject* UCombatMovesetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Create and return a new instance of our MyCustomAsset object
	UCombatMoveset* CombatMovesetAsset = NewObject<UCombatMoveset>(InParent, Class, Name, Flags);
	return CombatMovesetAsset;
}