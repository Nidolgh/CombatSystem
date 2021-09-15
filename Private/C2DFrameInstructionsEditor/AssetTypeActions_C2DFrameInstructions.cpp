#include "Combat2DEditor/Classes/AssetTypeActions_C2DFrameInstructions.h"
#include "Combat2DEditor/Classes/C2DFrameInstructions.h"

#include "Combat2DModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_C2DFrameInstructions"

FText FAssetTypeActions_C2DFrameInstructions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions_C2DFrameInstructions", "AssetTypeActions_C2DFrameInstructions", "C2DFrameInstructions");
}

FColor FAssetTypeActions_C2DFrameInstructions::GetTypeColor() const
{
	return FColor::Magenta;
}

UClass* FAssetTypeActions_C2DFrameInstructions::GetSupportedClass() const
{
	return UC2DFrameInstructions::StaticClass();
}

void FAssetTypeActions_C2DFrameInstructions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto C2DFrameInstructions = Cast<UC2DFrameInstructions>(*ObjIt);
		if (C2DFrameInstructions != NULL)
		{
			ICombat2DModule* C2DFrameInstructionsEditorModule = &FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
			C2DFrameInstructionsEditorModule->CreateC2DFrameInstructionsEditor(Mode, EditWithinLevelEditor, C2DFrameInstructions);
		}
	}
}

uint32 FAssetTypeActions_C2DFrameInstructions::GetCategories()
{
	const ICombat2DModule& MovesetEditorModule = FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
	return MovesetEditorModule.GetAssetCategoryType();
}

#undef LOCTEXT_NAMESPACE
