#include "Combat2DEditor/Classes/MovesetFactory.h"
#include "Combat2DEditor/Classes/Moveset.h"

UMovesetFactory::UMovesetFactory()
{
	// Provide the factory with information about how to handle our asset
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UMoveset::StaticClass();
}

UObject* UMovesetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Create and return a new instance of our MyCustomAsset object
	UMoveset* Moveset = NewObject<UMoveset>(InParent, Class, Name, Flags);
	return Moveset;
}