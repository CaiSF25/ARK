// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingSlot.h"
#include "CraftingStructs.h"
#include "ARK/Inventory/ItemInfo.h"
#include "Blueprint/UserWidget.h"
#include "CraftingContainerGrid.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UCraftingContainerGrid : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (BindWidget))
	class UUniformGridPanel* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Datatable")
	UDataTable* PlayerItemRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn))
	EContainerType ContainerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn))
	ECraftingType CraftingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn))
	EItemCategory ItemCategory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCraftingSlot> CraftingSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TArray<UCraftingSlot*> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	int32 SlotsPerRow;

	void AddSlots(const ECraftingType LocalCraftingType);

	void AddSlotToGrid(int32 Index, UCraftingSlot* SlotToAdd);
};
