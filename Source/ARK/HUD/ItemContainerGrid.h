// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySlot.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "ItemContainerGrid.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EContainerType : uint8
{
	PlayerInventory     UMETA(DisplayName = "PlayerInventory"),
	PlayerHotbar        UMETA(DisplayName = "PlayerHotbar"),
	PlayerStorage       UMETA(DisplayName = "PlayerStorage"),
	PlayerArmor         UMETA(DisplayName = "PlayerArmor"),
};

UCLASS()
class ARK_API UItemContainerGrid : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (BindWidget))
	class UUniformGridPanel* Grid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	int32 SlotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	int32 TotalSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Setting", meta = (ExposeOnSpawn = true))
	EContainerType ContainerType;

	UFUNCTION(BlueprintCallable)
	void AddSlots(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddSlotToGrid(int32 Index, UInventorySlot* Slots);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	TSubclassOf<UInventorySlot> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<UInventorySlot*> CreatedWidgets;
	
};
