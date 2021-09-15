// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Combat2DEditor/Classes/C2DFrameInstructions.h"

//////////////////////////////////////////////////////////////////////////
// STimelineHeader

// This is the bar above the timeline which (will someday) shows the frame ticks and current time
class STimelineHeader : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimelineHeader)
		: _SlateUnitsPerFrame(1)
		, _C2DFrameInstructionsBeingEdited(nullptr)
		, _PlayTime(0)
	{}

	SLATE_ATTRIBUTE(float, SlateUnitsPerFrame)
		SLATE_ATTRIBUTE(class UC2DFrameInstructions*, C2DFrameInstructionsBeingEdited)
		SLATE_ATTRIBUTE(float, PlayTime)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
	{
		SlateUnitsPerFrame = InArgs._SlateUnitsPerFrame;
		C2DFrameInstructionsBeingEdited = InArgs._C2DFrameInstructionsBeingEdited;
		PlayTime = InArgs._PlayTime;

		NumFramesFromLastRebuild = 0;

		ChildSlot
			[
				SAssignNew(MainBoxPtr, SHorizontalBox)
			];

		Rebuild();
	}

	void Rebuild()
	{
		MainBoxPtr->ClearChildren();

		UPaperFlipbook* Flipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook;
		const float LocalSlateUnitsPerFrame = SlateUnitsPerFrame.Get();
		if ((Flipbook != nullptr) && (LocalSlateUnitsPerFrame > 0))
		{
			const int32 NumFrames = Flipbook->GetNumFrames();
			for (int32 FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
			{
				MainBoxPtr->AddSlot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(LocalSlateUnitsPerFrame)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::AsNumber(FrameIndex))
					]
					];
			}

			NumFramesFromLastRebuild = NumFrames;
		}
		else
		{
			NumFramesFromLastRebuild = 0;
		}
	}

private:
	TAttribute<float> SlateUnitsPerFrame;
	TAttribute<class UC2DFrameInstructions*> C2DFrameInstructionsBeingEdited;
	TAttribute<float> PlayTime;

	TSharedPtr<SHorizontalBox> MainBoxPtr;

	int32 NumFramesFromLastRebuild;
};
