#pragma once

#include "CoreMinimal.h"
#include "CombatMoveset.h"

#include "CombatMovesetComponent.generated.h"

UCLASS()
class COMBAT2D_API UCombatMovesetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatMovesetComponent();

	UPROPERTY(EditAnywhere, Category="CombatMoveset")
	UCombatMoveset* CurrentCombatMoveset;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};