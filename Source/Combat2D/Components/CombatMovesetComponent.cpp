#include "CombatMovesetComponent.h"

UCombatMovesetComponent::UCombatMovesetComponent()
{
	
}

void UCombatMovesetComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatMovesetComponent::TickComponent(float DeltaTime, ELevelTick Tick,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, Tick, ThisTickFunction);
}
