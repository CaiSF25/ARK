// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/Structures/ItemInfo.h"
#include "Components/ActorComponent.h"
#include "ItemContainer.generated.h"


struct FItemStructure;

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), BlueprintType)
class ARK_API UItemContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemContainer();
	// 基础功能
	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 FindEmptySlot() const;

	UFUNCTION(BlueprintPure)
	FItemInfo GetItemAtIndex(const int32 Index) const;

	TArray<int32> GetIndexesOfItem(const int32 ItemID) const;

	// 物品操作
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(const FItemInfo& Item);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory")
	void ServerAddItem(const FItemInfo& Item);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual bool AddItemToIndex(FItemInfo Item, int32 LocalSpecificIndex, int32 FromIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual bool RemoveItemAtIndex(const int32 Index);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SwapItems(int32 SlotIndexA, int32 SlotIndexB);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool MergeStacks(int32 FromSlotIndex, int32 ToSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SplitStack(int32 SlotIndex, int32 SplitNum) const;

	// UI交互
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UpdateUI(int32 Index, const FItemInfo& Item, bool ResetSlot) const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory")
	void ServerOnSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex);

	void TransferItem(UItemContainer* ToComponent, const int32 ToSpecificIndex, const int32 ItemIndexToTransfer);

	EContainerType GetContainerType() const { return ContainerType; }

	TArray<FItemInfo> GetItems() const { return Items; }

	UFUNCTION()
	bool ContainsItems(TArray<FItemStructure>& RequiredItems);

	UFUNCTION()
	void RemoveItems(TArray<FItemStructure>& ItemsToRemove);
	
	bool UpdateItemQuantity(int32 Index, int32 NewQuantity);

	std::pair<bool, int32> RemoveQuantity(int32 Index, int32 AmountToRemove);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FItemInfo> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	EContainerType ContainerType;

	// 在子类中重写
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void HandleSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex);

private:
	// 基础功能
	UFUNCTION(BlueprintPure)
	bool IsSlotEmpty(int32 SlotIndex) const;

	UFUNCTION()
	bool HasItemsToStack(const FItemInfo& ItemInfo) const;
};
