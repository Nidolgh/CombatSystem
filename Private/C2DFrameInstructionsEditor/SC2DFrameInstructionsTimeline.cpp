// Copyright Epic Games, Inc. All Rights Reserved.

#include "SC2DFrameInstructionsTimeline.h"
#include "Rendering/DrawElements.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Colors/SColorBlock.h"
#include "EditorStyleSet.h"
#include "C2DFrameInstructionsEditorCommands.h"
#include "PaperFlipbook.h"
#include "STimelineHeader.h"

#include "DragAndDrop/AssetDragDropOp.h"
#include "ScopedTransaction.h"
#include "PaperSprite.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

//////////////////////////////////////////////////////////////////////////
// Inline widgets

#include "STimelineTrack.h"

//////////////////////////////////////////////////////////////////////////
// SFlipbookTimeline

void SC2DFrameInstructionsTimeline::Construct(const FArguments& InArgs, TSharedPtr<FUICommandList> InCommandList)
{
	C2DFrameInstructionsBeingEdited = InArgs._C2DFrameInstructionsBeingEdited;
	PlayTime = InArgs._PlayTime;
	OnSelectionChanged = InArgs._OnSelectionChanged;
	CommandList = InCommandList;

	SlateUnitsPerFrame = 120.0f;

	BackgroundPerFrameSlices = SNew(SHorizontalBox);

	TimelineHeader = SNew(STimelineHeader)
		.SlateUnitsPerFrame(this, &SC2DFrameInstructionsTimeline::GetSlateUnitsPerFrame)
		.C2DFrameInstructionsBeingEdited(C2DFrameInstructionsBeingEdited)
		.PlayTime(PlayTime);

	TimelineTrack = SNew(SFlipbookTimelineTrack, CommandList)
		.SlateUnitsPerFrame(this, &SC2DFrameInstructionsTimeline::GetSlateUnitsPerFrame)
		.C2DFrameInstructionsBeingEdited(C2DFrameInstructionsBeingEdited)
		.OnSelectionChanged(OnSelectionChanged);

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SScrollBox)
			.Orientation(Orient_Horizontal)
		.ScrollBarAlwaysVisible(true)
		.OnUserScrolled(this, &SC2DFrameInstructionsTimeline::AnimationScrollBar_OnUserScrolled)
		+SScrollBox::Slot()
		[
			SNew(SOverlay)

			// Per-frame background
		+SOverlay::Slot()
		.VAlign(VAlign_Fill)
		[
			BackgroundPerFrameSlices.ToSharedRef()
		]

	// Flipbook header and track
	+SOverlay::Slot()
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0,0,0,2)
		[
			TimelineHeader.ToSharedRef()
		]

	+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.HeightOverride(FFlipbookUIConstants::FrameHeight)
		[
			TimelineTrack.ToSharedRef()
		]
		]
		]

	// Empty flipbook instructions
	+ SOverlay::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Visibility(this, &SC2DFrameInstructionsTimeline::NoFramesWarningVisibility)
		.Text(LOCTEXT("EmptyTimelineInstruction", "Right-click here or drop in sprites to add key frames"))
		]
		]
		]
		];

	UPaperFlipbook* Flipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook;
	NumKeyFramesFromLastRebuild = (Flipbook != nullptr) ? Flipbook->GetNumKeyFrames() : 0;
	NumFramesFromLastRebuild = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;
	RebuildPerFrameBG();
}

void SC2DFrameInstructionsTimeline::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	SCompoundWidget::OnDragEnter(MyGeometry, DragDropEvent);

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
	}
	else if (Operation->IsOfType<FFlipbookKeyFrameDragDropOp>())
	{
		const auto& FrameDragDropOp = StaticCastSharedPtr<FFlipbookKeyFrameDragDropOp>(Operation);
		FrameDragDropOp->SetCanDropHere(true);
	}
}

void SC2DFrameInstructionsTimeline::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	SCompoundWidget::OnDragLeave(DragDropEvent);

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
	}
	else if (Operation->IsOfType<FFlipbookKeyFrameDragDropOp>())
	{
		const auto& FrameDragDropOp = StaticCastSharedPtr<FFlipbookKeyFrameDragDropOp>(Operation);
		FrameDragDropOp->SetCanDropHere(false);
	}
}

FReply SC2DFrameInstructionsTimeline::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	bool bWasDropHandled = false;

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
	}
	else if (Operation->IsOfType<FAssetDragDropOp>())
	{
		const auto& AssetDragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
		OnAssetsDropped(*AssetDragDropOp);
		bWasDropHandled = true;
	}
	else if (Operation->IsOfType<FFlipbookKeyFrameDragDropOp>())
	{
		const auto& FrameDragDropOp = StaticCastSharedPtr<FFlipbookKeyFrameDragDropOp>(Operation);
		if (UPaperFlipbook* ThisFlipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook)
		{
			FrameDragDropOp->AppendToFlipbook(ThisFlipbook);
			bWasDropHandled = true;
		}
	}

	return bWasDropHandled ? FReply::Handled() : FReply::Unhandled();
}

void SC2DFrameInstructionsTimeline::OnAssetsDropped(const class FAssetDragDropOp& DragDropOp)
{
	//@TODO: Support inserting in addition to dropping at the end
	TArray<FPaperFlipbookKeyFrame> NewFrames;
	for (const FAssetData& AssetData : DragDropOp.GetAssets())
	{
		if (UObject* Object = AssetData.GetAsset())
		{
			if (UPaperSprite* SpriteAsset = Cast<UPaperSprite>(Object))
			{
				// Insert this sprite as a keyframe
				FPaperFlipbookKeyFrame& NewFrame = *new (NewFrames) FPaperFlipbookKeyFrame();
				NewFrame.Sprite = SpriteAsset;
			}
			else if (UPaperFlipbook* FlipbookAsset = Cast<UPaperFlipbook>(Object))
			{
				// Insert all of the keyframes from the other flipbook into this one
				for (int32 KeyIndex = 0; KeyIndex < FlipbookAsset->GetNumKeyFrames(); ++KeyIndex)
				{
					const FPaperFlipbookKeyFrame& OtherFlipbookFrame = FlipbookAsset->GetKeyFrameChecked(KeyIndex);
					FPaperFlipbookKeyFrame& NewFrame = *new (NewFrames) FPaperFlipbookKeyFrame();
					NewFrame = OtherFlipbookFrame;
				}
			}
		}
	}

	UPaperFlipbook* ThisFlipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook;
	if (NewFrames.Num() && (ThisFlipbook != nullptr))
	{
		const FScopedTransaction Transaction(LOCTEXT("DroppedAssetOntoTimelineTransaction", "Insert assets as frames"));
		ThisFlipbook->Modify();

		FScopedFlipbookMutator EditLock(ThisFlipbook);
		EditLock.KeyFrames.Append(NewFrames);
	}
}

int32 SC2DFrameInstructionsTimeline::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const float CurrentTimeSecs = PlayTime.Get();
	UPaperFlipbook* Flipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook;
	const float TotalTimeSecs = (Flipbook != nullptr) ? Flipbook->GetTotalDuration() : 0.0f;
	const int32 TotalNumFrames = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;

	const float SlateTotalDistance = SlateUnitsPerFrame * TotalNumFrames;
	const float CurrentTimeXPos = (((CurrentTimeSecs / TotalTimeSecs) * SlateTotalDistance) - AnimationScrollBarPosition) + FMath::Clamp((AllottedGeometry.GetLocalSize().X + AnimationScrollBarPosition) - SlateTotalDistance, 0.0f, AnimationScrollBarPosition);

	// Draw a line for the current scrub cursor
	++LayerId;
	TArray<FVector2D> LinePoints;
	LinePoints.Add(FVector2D(CurrentTimeXPos, 0.f));
	LinePoints.Add(FVector2D(CurrentTimeXPos, AllottedGeometry.GetLocalSize().Y));

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		LinePoints,
		ESlateDrawEffect::None,
		FLinearColor::Red
	);

	return LayerId;
}

FReply SC2DFrameInstructionsTimeline::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		const float DirectionScale = 0.08f;
		const float MinFrameSize = 16.0f;
		const float Direction = MouseEvent.GetWheelDelta();
		const float NewUnitsPerFrame = FMath::Max(MinFrameSize, SlateUnitsPerFrame * (1.0f + Direction * DirectionScale));
		SlateUnitsPerFrame = NewUnitsPerFrame;

		CheckForRebuild(/*bRebuildAll=*/ true);

		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

TSharedRef<SWidget> SC2DFrameInstructionsTimeline::GenerateContextMenu()
{
	FMenuBuilder MenuBuilder(true, CommandList);
	MenuBuilder.BeginSection("KeyframeActions", LOCTEXT("KeyframeActionsSectionHeader", "Keyframe Actions"));

	// 		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Cut);
	// 		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);
	// 		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
	MenuBuilder.AddMenuEntry(FC2DFrameInstructionsEditorCommands::Get().AddNewFrame);

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

FReply SC2DFrameInstructionsTimeline::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		TSharedRef<SWidget> MenuContents = GenerateContextMenu();
		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

void SC2DFrameInstructionsTimeline::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	CheckForRebuild();
}

void SC2DFrameInstructionsTimeline::CheckForRebuild(bool bRebuildAll)
{
	UPaperFlipbook* Flipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook;

	const int32 NewNumKeyframes = (Flipbook != nullptr) ? Flipbook->GetNumKeyFrames() : 0;
	if ((NewNumKeyframes != NumKeyFramesFromLastRebuild) || bRebuildAll)
	{
		NumKeyFramesFromLastRebuild = NewNumKeyframes;
		TimelineTrack->Rebuild();
	}

	const int32 NewNumFrames = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;
	if ((NewNumFrames != NumFramesFromLastRebuild) || bRebuildAll)
	{
		NumFramesFromLastRebuild = NewNumFrames;
		TimelineHeader->Rebuild();
		RebuildPerFrameBG();
	}
}

EVisibility SC2DFrameInstructionsTimeline::NoFramesWarningVisibility() const
{
	UPaperFlipbook* Flipbook = C2DFrameInstructionsBeingEdited.Get()->TargetFlipbook;
	const int32 TotalNumFrames = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;
	return (TotalNumFrames == 0) ? EVisibility::Visible : EVisibility::Collapsed;
}

void SC2DFrameInstructionsTimeline::RebuildPerFrameBG()
{
	const FLinearColor BackgroundColors[2] = { FLinearColor(1.0f, 1.0f, 1.0f, 0.05f), FLinearColor(0.0f, 0.0f, 0.0f, 0.05f) };

	BackgroundPerFrameSlices->ClearChildren();
	for (int32 FrameIndex = 0; FrameIndex < NumFramesFromLastRebuild; ++FrameIndex)
	{
		const FLinearColor& BackgroundColorForFrameIndex = BackgroundColors[FrameIndex & 1];

		BackgroundPerFrameSlices->AddSlot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(SlateUnitsPerFrame)
			[
				SNew(SColorBlock)
				.Color(BackgroundColorForFrameIndex)
			]
			];
	}
}

void SC2DFrameInstructionsTimeline::AnimationScrollBar_OnUserScrolled(float ScrollOffset)
{
	AnimationScrollBarPosition = ScrollOffset;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
