// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"

#include "CombatFlipbookComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBAT2D_API UCombatFlipbookComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatFlipbookComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// UPrimitiveComponent interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual class UBodySetup* GetBodySetup() override;
	// End of UPrimitiveComponent interface
	
	FLinearColor GetWireframeColor() const;

	void SetActiveCombatFlipbook(class UCombatFlipbook* NewActiveCombatFlipbook);
	void SetPaperFlipbookComponent(UPaperFlipbookComponent* NewPaperFlipbookComp);

	void SetActiveBodySetupCombatFrame();

	UPROPERTY(EditAnywhere, Category="Main")
		class UBodySetup* ActiveBodySetup;
	
	UPROPERTY(EditAnywhere, Category="Main")
	class UCombatFlipbook* ActiveCombatFlipbook;

	UPROPERTY(EditAnywhere, Category="Main")
	float OverwriteUnrealUnitsPerPixel;

protected:
	const UPaperFlipbookComponent* OwnerPaperFlipbookComp;
	
	class FCombatFlipbookSceneProxy* CombatFlipbookSceneProxy;

	int32 PreviousFrameIndex;
};
