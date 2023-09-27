#pragma once

#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"

class UAnimNotifyState_CombatSystemCollision;

class FCombatSystemCollisionDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FCombatSystemCollisionDetails);
	}
	
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:
	void DrawCombatSystemDetails(UAnimNotifyState_CombatSystemCollision* NotifyEvent, IDetailLayoutBuilder& DetailBuilder) const;
};
