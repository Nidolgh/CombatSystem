#include "SCombatFlipbookDetailsTab.h"

#include "CombatFlipbookEditor/CombatFlipbookEditor.h"

void SCombatFlipbookDetailsTab::Construct(const FArguments& InArgs,
                TSharedPtr<FCombatFlipbookEditor> InFlipbookEditor)
{
    CombatFlipbookEditorPtr = InFlipbookEditor;

    SSingleObjectDetailsPanel::Construct(
        SSingleObjectDetailsPanel::FArguments().
        HostCommandList(InFlipbookEditor->GetToolkitCommands()).
        HostTabManager(InFlipbookEditor->GetTabManager()),
        /*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/
        true);
}

UObject* SCombatFlipbookDetailsTab::GetObjectToObserve() const
{
    return CombatFlipbookEditorPtr.Pin()->GetCombatFlipbookBeingEdited();
}

 TSharedRef<SWidget> SCombatFlipbookDetailsTab::PopulateSlot(
    TSharedRef<SWidget> PropertyEditorWidget)
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .FillHeight(1)
        [
            PropertyEditorWidget
        ];
}
