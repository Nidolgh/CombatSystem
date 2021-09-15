#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "C2DFrameInstructionsEditorSettings.generated.h"

// Settings for the C2DFrameInstructions editor
UCLASS(config=EditorPerProjectUserSettings)
class UC2DFrameInstructionsEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UC2DFrameInstructionsEditorSettings();

	/** Background color in the C2DFrameInstructions editor */
	UPROPERTY(config, EditAnywhere, Category=Background, meta=(HideAlphaChannel))
	FColor BackgroundColor;

	/** Should the grid be shown by default when the editor is opened? */
	UPROPERTY(config, EditAnywhere, Category=Background)
	bool bShowGridByDefault;
};
