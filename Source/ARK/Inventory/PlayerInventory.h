// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "PlayerInventory.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), BlueprintType)
class ARK_API UPlayerInventory : public UItemContainer
{
	GENERATED_BODY()
	
public:
	UPlayerInventory();
};
