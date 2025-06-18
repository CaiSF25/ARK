// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/Character/SurvivalPlayerController.h"
#include "UObject/Interface.h"
#include "SurvivalCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USurvivalCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARK_API ISurvivalCharacterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	ASurvivalPlayerController* GetControllerFromChar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	class ASurvivalCharacter* GetSurvivalCharRef();
};
