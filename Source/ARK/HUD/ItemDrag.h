// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/Inventory/ItemInfo.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDrag.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UItemDrag : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag", meta = (ExposeOnSpawn))
	int32 SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag", meta = (ExposeOnSpawn))
	EContainerType FromContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag", meta = (ExposeOnSpawn))
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drag", meta = (ExposeOnSpawn))
	EArmorType ArmorType;
};
