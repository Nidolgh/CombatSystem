#include "Combat2DEditor/Classes/FlipbookDataFactory.h"
#include "Combat2DEditor/Classes/FlipbookData.h"

UFlipbookDataFactory::UFlipbookDataFactory()
{
	// Provide the factory with information about how to handle our asset
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UFlipbookData::StaticClass();
}

UObject* UFlipbookDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Create and return a new instance of our MyCustomAsset object
	UFlipbookData* FlipbookData = NewObject<UFlipbookData>(InParent, Class, Name, Flags);
	return FlipbookData;
}