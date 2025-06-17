// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySlot.h"
#include "ARK/Inventory/ItemInfo.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "ItemContainerGrid.generated.h"

/**
 * 
 */

UCLASS()
class ARK_API UItemContainerGrid : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	
	
public:
	UFUNCTION(BlueprintCallable)
	void AddSlots(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddSlotToGrid(int32 Index, UInventorySlot* LocalSlots);

	UFUNCTION(BlueprintCallable)
	const TArray<UInventorySlot*>& GetSlots() const { return Slots; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (BindWidget))
	class UUniformGridPanel* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	int32 SlotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	int32 TotalSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	EContainerType ContainerType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<UInventorySlot*> Slots;

private:
	UPROPERTY(EditAnywhere, Category = "Items")
	TSubclassOf<UInventorySlot> WidgetClass;
};
