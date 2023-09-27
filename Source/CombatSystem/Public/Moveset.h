// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Moveset.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UMoveset : public UObject
{
	GENERATED_BODY()
	
public:
	UMoveset();
 
	UPROPERTY(EditAnywhere)
	float Mean;
 
	UPROPERTY(EditAnywhere)
	float StandardDeviation;
 
	UFUNCTION(BlueprintCallable)
	float DrawSample();
 
	UFUNCTION(CallInEditor)
	void LogSample();
};
