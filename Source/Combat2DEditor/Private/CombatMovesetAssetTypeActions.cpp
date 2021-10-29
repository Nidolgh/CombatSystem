#include "CombatMovesetAssetTypeActions.h"
#include "CombatMoveset.h"

#include "Combat2DEditorModule.h"

#define LOCTEXT_NAMESPACE "CombatMovesetAssetTypeActions"

FText FCombatMovesetAssetTypeActions::GetName() const
{
	return NSLOCTEXT("CombatMovesetAssetTypeActions", "CombatMovesetAssetTypeActions", "CombatMoveset");
}

FColor FCombatMovesetAssetTypeActions::GetTypeColor() const
{
	return FColor::Magenta;
}

UClass* FCombatMovesetAssetTypeActions::GetSupportedClass() const
{
	return UCombatMoveset::StaticClass();
}

void FCombatMovesetAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto CombatMoveset = Cast<UCombatMoveset>(*ObjIt);
		if (CombatMoveset != NULL)
		{
			FCombat2DEditorModule* MovesetEditorModule = &FModuleManager::LoadModuleChecked<FCombat2DEditorModule>("Combat2DEditor");
			MovesetEditorModule->CreateCombatMovesetEditor(Mode, EditWithinLevelEditor, CombatMoveset);
		}
	}
}

uint32 FCombatMovesetAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE
