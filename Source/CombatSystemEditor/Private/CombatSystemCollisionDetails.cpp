#include "CombatSystemCollisionDetails.h"

#include "AnimNotifyState_CombatSystemCollision.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Animation/EditorNotifyObject.h"

#define LOCTEXT_NAMESPACE "CombatSystemCollisionDetails"

void FCombatSystemCollisionDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// We only want to continue when we have only one of our target CombatSystemCollision NotifyEventState selected
	{
		const TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailBuilder.GetSelectedObjects();
		if (SelectedObjects.IsEmpty() || SelectedObjects.Num() > 1)
		{
			return;
		}

		const UEditorNotifyObject* NotifyObject = Cast<UEditorNotifyObject>(SelectedObjects[0]);

		if (NotifyObject == nullptr || NotifyObject->Event.NotifyStateClass == nullptr)
		{
			return;
		}
	
		UAnimNotifyState_CombatSystemCollision* NotifyEvent = Cast<UAnimNotifyState_CombatSystemCollision>(NotifyObject->Event.NotifyStateClass);
		if (NotifyEvent == nullptr)
		{
			return;
		}

		DrawCombatSystemDetails(NotifyEvent, DetailBuilder);
	}
}

void FCombatSystemCollisionDetails::DrawCombatSystemDetails(UAnimNotifyState_CombatSystemCollision* NotifyEvent, IDetailLayoutBuilder& DetailBuilder) const
{
	IDetailCategoryBuilder& AnimNotifyCategory = DetailBuilder.EditCategory("CombatSystem");
	const FText Test = LOCTEXT("CombatSystemCollisionDetails_NewRow", "Hello Row!");
	AnimNotifyCategory.AddCustomRow(Test)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "Button")
				.OnClicked_Lambda([this, &DetailBuilder]() -> FReply
				{
					const UEditorNotifyObject* NotifyObject = Cast<UEditorNotifyObject>(DetailBuilder.GetSelectedObjects()[0]);
					UAnimNotifyState_CombatSystemCollision* NotifyEvent = Cast<UAnimNotifyState_CombatSystemCollision>(NotifyObject->Event.NotifyStateClass);

					NotifyEvent->AddShapeType(ECombatSystemCollisionShapeType::Box);
					NotifyEvent->CreateUniqueShapeComponents();
						
					return FReply::Handled();
				})
				.Text(LOCTEXT("CombatSystemCollisionDetails", "Add Box Shape"))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "Button")
				.OnClicked_Lambda([this, &DetailBuilder]() -> FReply
				{
					const UEditorNotifyObject* NotifyObject = Cast<UEditorNotifyObject>(DetailBuilder.GetSelectedObjects()[0]);
					UAnimNotifyState_CombatSystemCollision* NotifyEvent = Cast<UAnimNotifyState_CombatSystemCollision>(NotifyObject->Event.NotifyStateClass);

					NotifyEvent->AddShapeType(ECombatSystemCollisionShapeType::Sphere);
					NotifyEvent->CreateUniqueShapeComponents();

					return FReply::Handled();
				})
				.Text(LOCTEXT("CombatSystemCollisionDetails", "Add Sphere Shape"))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.ContentPadding(0)
				.ButtonStyle(FAppStyle::Get(), "Button")
				.OnClicked_Lambda([this, &DetailBuilder]() -> FReply
				{
					const UEditorNotifyObject* NotifyObject = Cast<UEditorNotifyObject>(DetailBuilder.GetSelectedObjects()[0]);
					UAnimNotifyState_CombatSystemCollision* NotifyEvent = Cast<UAnimNotifyState_CombatSystemCollision>(NotifyObject->Event.NotifyStateClass);
						
					NotifyEvent->AddShapeType(ECombatSystemCollisionShapeType::Capsule);
					NotifyEvent->CreateUniqueShapeComponents();

					return FReply::Handled();
				})
				.Text(LOCTEXT("CombatSystemCollisionDetails", "Add Capsule Shape"))
			]
		] + SVerticalBox::Slot()
		[
			SNew(SButton)
			.ContentPadding(0)
			.ButtonStyle(FAppStyle::Get(), "Button")
			.OnClicked_Lambda([this, &DetailBuilder]() -> FReply
			{
				const UEditorNotifyObject* NotifyObject = Cast<UEditorNotifyObject>(DetailBuilder.GetSelectedObjects()[0]);
				UAnimNotifyState_CombatSystemCollision* NotifyEvent = Cast<UAnimNotifyState_CombatSystemCollision>(NotifyObject->Event.NotifyStateClass);
				
				NotifyEvent->ClearCollisionShapeInfoArray();

				return FReply::Handled();
			})
			.Text(LOCTEXT("CombatSystemCollisionDetails", "Clear All Shapes"))
		]];
}
