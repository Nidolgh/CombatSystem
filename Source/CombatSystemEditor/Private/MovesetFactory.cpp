// Fill out your copyright notice in the Description page of Project Settings.


#include "MovesetFactory.h"

#include "Moveset.h"

UMovesetFactory::UMovesetFactory()
{
	SupportedClass = UMoveset::StaticClass();
	bCreateNew = true;
}

UObject* UMovesetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                           UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMoveset>(InParent, InClass, InName, Flags, Context);
}
