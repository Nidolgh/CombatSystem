#include "MovesetAssetTypeActions.h"

#include "Moveset.h"

UClass* FMovesetAssetTypeActions::GetSupportedClass() const
{
	return UMoveset::StaticClass();
}

FText FMovesetAssetTypeActions::GetName() const
{
	return INVTEXT("Moveset");
}

FColor FMovesetAssetTypeActions::GetTypeColor() const
{
	return FColor::Cyan;
}

uint32 FMovesetAssetTypeActions::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("CombatSystem"));
	}
	return EAssetTypeCategories::Misc;
}
