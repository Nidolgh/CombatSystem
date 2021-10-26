#include "SFrameCollisionDataTab.h"

#include "CombatFlipbook.h"
#include "CombatFlipbookEditor/CombatFlipbookEditor.h"

void SFrameCollisionDataTab::Construct(const FArguments &InArgs, 
        TSharedPtr<FCombatFlipbookEditor> InFlipbookEditor)
{
    CombatFlipbookEditorPtr = InFlipbookEditor;

    ChildSlot
        [
            SAssignNew(MainBoxPtr, SVerticalBox)
        ];

    Rebuild();
}

void SFrameCollisionDataTab::Rebuild()
{
    UCombatFlipbook* CombatFlipbook = CombatFlipbookEditorPtr.Pin()->GetCombatFlipbookBeingEdited();
    if (CombatFlipbook->CombatFramesArray.Num() == 0)
    {
        return;
    }
    
    KFArray = &CombatFlipbook->CombatFramesArray[CombatFlipbookEditorPtr.Pin()->GetCurrentFrame()].CollisionDataArray;

    MainBoxPtr->ClearChildren();
    //HorizontalButtonsPtr->ClearChildren();

    FString HeaderBase = TEXT("Active instruction frame");
    HeaderBase.Append("_");
    HeaderBase.Append(FString::FromInt(CombatFlipbookEditorPtr.Pin()->GetCurrentFrame()));
    HeaderBase.Append("-");
    HeaderBase.Append(FString::FromInt(ButtonFrameInstructionID));

    if (KFArray->Num() == 0)
    {
        HeaderBase = TEXT("No instruction frames");
    }
    
    MainBoxPtr->AddSlot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString(HeaderBase))
        ];
    MainBoxPtr->AddSlot()
        .AutoHeight()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Add Instruction")))
            .OnClicked_Lambda([this]()
                {
                    CombatFlipbookEditorPtr.Pin().Get()->CreateKeyFrameInstructionOnCurrentFrame();

                    return FReply::Handled();
                })
        ];
    MainBoxPtr->AddSlot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString("Instructions:"))
        ];
    MainBoxPtr->AddSlot()
        .AutoHeight()[
            SAssignNew(HorizontalButtonsPtr, SHorizontalBox)
        ];
    MainBoxPtr->AddSlot()
        .AutoHeight()[
            SNew(SScrollBar)
        ];
    
    if (KFArray != nullptr)
    {
        for (size_t i = 0; i < KFArray->Num(); i++)
        {
            FString ButtonText = TEXT("");
            ButtonText.Append(FString::FromInt(i));

            FLinearColor ButtonCol(0.5f, 0.5f, 0.5f, 1.f);
            if (i == ButtonFrameInstructionID)
            {
                ButtonCol = FLinearColor::White;
            }
            
            HorizontalButtonsPtr.Get()->AddSlot()
                .AutoWidth()
                [
                    SNew(SButton)
                    .ClickMethod(EButtonClickMethod::MouseDown)
                    .Text(FText::FromString(ButtonText))
                    .ButtonColorAndOpacity(FSlateColor(ButtonCol))
                    .OnClicked_Lambda([&, i]()
                    {
                        UE_LOG(LogTemp, Log, TEXT("Button pressed: %i"), i);

                        ButtonFrameInstructionID = i;

                        return FReply::Handled();
                    })
                ];
        }

        MainBoxPtr->AddSlot()
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Type:"))
            ];

        ECollisionType& CollisionTypeRef = KFArray->GetData()[ButtonFrameInstructionID].CollisionType;
        
        for (size_t i = 0; i < static_cast<size_t>(ECollisionType::Count); i++)
        {
            FString InsTypeString("");
            FLinearColor InsTypeLColor(FLinearColor::White);
            const ECollisionType CurType = static_cast<ECollisionType>(i);
            
            switch (CurType)
            {
            case ECollisionType::HitBox:
                InsTypeString = TEXT("HitBox");
                break;
            case ECollisionType::HurtBox:
                InsTypeString = TEXT("HurtBox");
                break;
            default: 
                InsTypeString = TEXT("Type undefined!");
            }

            if (CurType != CollisionTypeRef)
            {
                InsTypeLColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.f);
            }
            
            MainBoxPtr->AddSlot()
                .AutoHeight()[
                    SNew(SButton)
                        .Text(FText::FromString(InsTypeString))
                        .ButtonColorAndOpacity(FSlateColor(InsTypeLColor))
                        .OnClicked_Lambda([this, i, &CollisionTypeRef]() mutable
                        {
                            CollisionTypeRef = static_cast<ECollisionType>(i);

                            Rebuild();
                            
                            return FReply::Handled();
                        })
                ];
        }
    }
}

void SFrameCollisionDataTab::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    // check for rebuild CombatFlipbookEditorPtr.Pin()->GetCurrentFrame()
    if ( KFArray != nullptr
        && (( LastInstructionNum != KFArray->Num() ) 
        || ( LastFrame != CombatFlipbookEditorPtr.Pin()->GetCurrentFrame() )
        || ( LastButtonFrameInstructionID != ButtonFrameInstructionID ) )
        ) 
    {
        LastButtonFrameInstructionID = ( LastFrame == CombatFlipbookEditorPtr.Pin()->GetCurrentFrame() ) ? ButtonFrameInstructionID : 0;

        LastInstructionNum = KFArray->Num();
        LastFrame = CombatFlipbookEditorPtr.Pin()->GetCurrentFrame();

        Rebuild();
    }
    else if (KFArray == nullptr && CombatFlipbookEditorPtr.Pin()->GetCombatFlipbookBeingEdited()->CombatFramesArray.Num() > 0)
    {
        KFArray = &CombatFlipbookEditorPtr.Pin()->GetCombatFlipbookBeingEdited()->CombatFramesArray[CombatFlipbookEditorPtr.Pin()->GetCurrentFrame()].CollisionDataArray;
    }
}

int32 SFrameCollisionDataTab::GetButtonFrameID()
{
    return ButtonFrameInstructionID;
}
