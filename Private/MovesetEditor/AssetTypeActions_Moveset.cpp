#include "Combat2DEditor/Classes/AssetTypeActions_Moveset.h"
#include "Combat2DEditor/Classes/Moveset.h"

#include "Combat2DModule.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_Moveset"

FText FAssetTypeActions_Moveset::GetName() const
{
	return NSLOCTEXT("AssetTypeActions_Moveset", "AssetTypeActions_Moveset", "Moveset");
}

FColor FAssetTypeActions_Moveset::GetTypeColor() const
{
	return FColor::Magenta;
}

UClass* FAssetTypeActions_Moveset::GetSupportedClass() const
{
	return UMoveset::StaticClass();
}

void FAssetTypeActions_Moveset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Moveset = Cast<UMoveset>(*ObjIt);
		if (Moveset != NULL)
		{
			ICombat2DModule* MovesetEditorModule = &FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
			MovesetEditorModule->CreateMovesetEditor(Mode, EditWithinLevelEditor, Moveset);
		}
	}
}

uint32 FAssetTypeActions_Moveset::GetCategories()
{
	const ICombat2DModule& MovesetEditorModule = FModuleManager::LoadModuleChecked<ICombat2DModule>("Combat2DEditor");
	return MovesetEditorModule.GetAssetCategoryType();
}

#undef LOCTEXT_NAMESPACE
