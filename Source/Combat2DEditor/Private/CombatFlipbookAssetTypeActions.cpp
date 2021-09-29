#include "CombatFlipbookAssetTypeActions.h"
#include "CombatFlipbook.h"

#include "Combat2DEditorModule.h"

#define LOCTEXT_NAMESPACE "CombatFlipbookAssetTypeActions"

FText FCombatFlipbookAssetTypeActions::GetName() const
{
	return NSLOCTEXT("CombatFlipbookAssetTypeActions", "CombatFlipbookAssetTypeActions", "Combat Flipbook");
}

FColor FCombatFlipbookAssetTypeActions::GetTypeColor() const
{
	return FColor::Magenta;
}

UClass* FCombatFlipbookAssetTypeActions::GetSupportedClass() const
{
	return UCombatFlipbook::StaticClass();
}

void FCombatFlipbookAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
                UCombatFlipbook* CombatFlipbook = Cast<UCombatFlipbook>(*ObjIt);
		if (CombatFlipbook != NULL)
		{
			FCombat2DEditorModule* MovesetEditorModule = &FModuleManager::LoadModuleChecked<FCombat2DEditorModule>("Combat2DEditor");
			MovesetEditorModule->CreateCombatFlipbookEditor(Mode, EditWithinLevelEditor, CombatFlipbook);
		}
	}
}

uint32 FCombatFlipbookAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE
