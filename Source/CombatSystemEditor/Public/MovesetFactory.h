// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MovesetFactory.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEMEDITOR_API UMovesetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UMovesetFactory();
	
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
