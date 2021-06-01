#include "Combat2DEditor/Classes/AssetTypeActions_FlipbookData.h"
#include "Combat2DEditor/Classes/FlipbookData.h"

#include "Combat2DModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_FlipbookData"

FText FAssetTypeActions_FlipbookData::GetName() const
{
	return NSLOCTEXT("AssetTypeActions_FlipbookData", "AssetTypeActions_FlipbookData", "FlipbookData");
}

FColor FAssetTypeActions_FlipbookData::GetTypeColor() const
{
	return FColor::Magenta;
}

UClass* FAssetTypeActions_FlipbookData::GetSupportedClass() const
{
	return UFlipbookData::StaticClass();
}

void FAssetTypeActions_FlipbookData::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto FlipbookData = Cast<UFlipbookData>(*ObjIt);
		if (FlipbookData != NULL)
		{
			ICombat2DModule* FlipbookDataEditorModule = &FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
			FlipbookDataEditorModule->CreateFlipbookDataEditor(Mode, EditWithinLevelEditor, FlipbookData);
		}
	}
}

uint32 FAssetTypeActions_FlipbookData::GetCategories()
{
	const ICombat2DModule& MovesetEditorModule = FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
	return MovesetEditorModule.GetAssetCategoryType();
}

#undef LOCTEXT_NAMESPACE
