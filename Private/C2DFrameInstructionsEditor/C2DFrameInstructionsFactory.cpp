#include "Combat2DEditor/Classes/C2DFrameInstructionsFactory.h"
#include "Combat2DEditor/Classes/C2DFrameInstructions.h"

UC2DFrameInstructionsFactory::UC2DFrameInstructionsFactory()
{
	// Provide the factory with information about how to handle our asset
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UC2DFrameInstructions::StaticClass();
}

UObject* UC2DFrameInstructionsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Create and return a new instance of our MyCustomAsset object
	UC2DFrameInstructions* C2DFrameInstructions = NewObject<UC2DFrameInstructions>(InParent, Class, Name, Flags);
	return C2DFrameInstructions;
}