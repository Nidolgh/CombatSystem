#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FlipbookDataEditorSettings.generated.h"

// Settings for the FlipbookData editor
UCLASS(config=EditorPerProjectUserSettings)
class UFlipbookDataEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UFlipbookDataEditorSettings();

	/** Background color in the FlipbookData editor */
	UPROPERTY(config, EditAnywhere, Category=Background, meta=(HideAlphaChannel))
	FColor BackgroundColor;

	/** Should the grid be shown by default when the editor is opened? */
	UPROPERTY(config, EditAnywhere, Category=Background)
	bool bShowGridByDefault;
};
