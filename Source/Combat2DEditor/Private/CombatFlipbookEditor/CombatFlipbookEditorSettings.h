#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CombatFlipbookEditorSettings.generated.h"

// Settings for the CombatFlipbook editor
UCLASS(config=EditorPerProjectUserSettings)
class UCombatFlipbookEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UCombatFlipbookEditorSettings();

	/** Background color in the CombatFlipbook editor */
	UPROPERTY(config, EditAnywhere, Category=Background, meta=(HideAlphaChannel))
	FColor BackgroundColor;

	/** Should the grid be shown by default when the editor is opened? */
	UPROPERTY(config, EditAnywhere, Category=Background)
	bool bShowGridByDefault;
};
