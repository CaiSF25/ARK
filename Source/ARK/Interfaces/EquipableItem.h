// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipableItem.generated.h"

class AEquipableMaster;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEquipableItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARK_API IEquipableItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipableInterface")
	AEquipableMaster* GetEquipableRef();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipableInterface")
	void UseItemInterface(ASurvivalCharacter* CharRef);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipableInterface")
	void EndAnimation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "EquipableInterface")
	void NotifyInterface();
};
