// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainer.h"

#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Character/SurvivalPlayerController.h"
#include "ARK/Structures/CraftingStructs.h"

UItemContainer::UItemContainer()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

bool UItemContainer::AddItem(const FItemInfo& Item)
{
	// 不可堆叠物品
	if (!Item.IsStackable)
	{
		const int32 Index = FindEmptySlot();
		if (Index == -1) return false;
		Items[Index] = Item;
		UpdateUI(Index, Item, false);
		return true;
	}
	int32 Remaining = Item.ItemQuantity;

	// 可堆叠物品： 优先堆叠到现有槽位
	for (int32 Index = 0; Index < Items.Num() && Remaining > 0; Index++)
	{
		FItemInfo& ExistingItem = Items[Index];

		// 检查是否可堆叠
		if (ExistingItem.ItemID == Item.ItemID && ExistingItem.ItemQuantity < ExistingItem.StackSize)
		{
			const int32 SpaceLeft = ExistingItem.StackSize - ExistingItem.ItemQuantity;
			const int32 ToAdd = FMath::Min(SpaceLeft, Remaining);

			ExistingItem.ItemQuantity += ToAdd;
			Remaining -= ToAdd;
			UpdateUI(Index, ExistingItem, false);
		}
	}

	// 将剩余物品放入物品槽
	while (Remaining > 0)
	{
		const int32 SlotIndex = FindEmptySlot();
		if (SlotIndex == -1)
			return false;   // 空间不足

		const int32 ToAdd = FMath::Min(Remaining, Item.StackSize);
		FItemInfo NewItem = Item;
		NewItem.ItemQuantity  = ToAdd;

		Items[SlotIndex] = NewItem;
		Remaining -= ToAdd;
		UpdateUI(SlotIndex, NewItem, false);
	}
	return true;
}

void UItemContainer::ServerAddItem_Implementation(const FItemInfo& Item)
{
	AddItem(Item);
}

bool UItemContainer::AddItemToIndex(FItemInfo Item, int32 LocalSpecificIndex, int32 FromIndex)
{
	if (IsSlotEmpty(LocalSpecificIndex))
	{
		Items[LocalSpecificIndex] = Item;
		return true;
	}
	return false;
}

bool UItemContainer::RemoveItemAtIndex(const int32 Index)
{
	Items[Index] = FItemInfo();
	return true;
}

bool UItemContainer::SwapItems(int32 SlotIndexA, int32 SlotIndexB)
{
	return false;
}

bool UItemContainer::MergeStacks(int32 FromSlotIndex, int32 ToSlotIndex)
{
	return false;
}

bool UItemContainer::SplitStack(int32 SlotIndex, int32 SplitNum) const
{
	return false;
}

int32 UItemContainer::FindEmptySlot() const
{
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index].ItemID == 0)
		{
			return Index;
		}
	}
	return -1;
}

void UItemContainer::UpdateUI(int32 Index, const FItemInfo& Item, bool ResetSlot) const
{
	ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOwner());
	if (!Character || !Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		return;
	}

	ASurvivalPlayerController* PlayerController =
		Cast<ASurvivalPlayerController>(ISurvivalCharacterInterface::Execute_GetControllerFromChar(Character));
	if (!PlayerController)
	{
		return;
	}
	
	if (ResetSlot)
	{
		PlayerController->ResetItemSlot(ContainerType, Index);
	}
	else
	{
		PlayerController->UpdateItemSlot(ContainerType, Index, Item);
	}
}

void UItemContainer::TransferItem(UItemContainer* ToComponent, const int32 ToSpecificIndex, const int32 ItemIndexToTransfer)
{
	if (IsValid(ToComponent))
	{
		const FItemInfo ItemToTransfer = GetItemAtIndex(ItemIndexToTransfer);
		if (ToComponent->AddItemToIndex(ItemToTransfer, ToSpecificIndex, ItemIndexToTransfer))
		{
			RemoveItemAtIndex(ItemIndexToTransfer);
		}
	}
}

FItemInfo UItemContainer::GetItemAtIndex(const int32 Index) const
{
	return Items.IsValidIndex(Index) ? Items[Index] : FItemInfo();
}

bool UItemContainer::ContainsItems(TArray<FItemStructure>& RequiredItems)
{
	TMap<int32, int32> InventoryCount;
	for (const auto& Slot : Items)
	{
		InventoryCount.FindOrAdd(Slot.ItemID) += Slot.ItemQuantity;
	}

	for (const auto& Req : RequiredItems)
	{
		int32* FoundQty = InventoryCount.Find(Req.ItemID);

		if (!FoundQty || *FoundQty < Req.ItemQuantity)
		{
			return false;
		}
		*FoundQty -= Req.ItemQuantity;
	}
	return true;
}

void UItemContainer::RemoveItems(TArray<FItemStructure>& ItemsToRemove)
{
	TMap<int32, int32> RemoveMap; // ItemID -> Quantity
	for (const auto& Req : ItemsToRemove) {
		RemoveMap.FindOrAdd(Req.ItemID) += Req.ItemQuantity;
	}

	for (int32 Index = 0; Index < Items.Num() && RemoveMap.Num() > 0; Index++) {
		FItemInfo& Slot = Items[Index];
		if (int32* QtyPtr = RemoveMap.Find(Slot.ItemID)) {
			const int32 ToRemove = FMath::Min(*QtyPtr, Slot.ItemQuantity);
			Slot.ItemQuantity -= ToRemove;
			*QtyPtr -= ToRemove;

			if (Slot.ItemQuantity == 0) {
				Slot = FItemInfo(); // 清空槽位
				UpdateUI(Index, Slot, true);
			} else {
				UpdateUI(Index, Slot, false);
			}

			if (*QtyPtr == 0) RemoveMap.Remove(Slot.ItemID);
		}
	}
	/*for (const FItemStructure& Req : ItemsToRemove)
	{
		int32 RemainingToRemove = Req.ItemQuantity;
		TArray<int32> SlotIndexes = GetIndexesOfItem(Req.ItemID);
		
		for (const int32 SlotIdx : SlotIndexes)
		{
			int32 SlotQty = Items[SlotIdx].ItemQuantity;

			if (SlotQty <= RemainingToRemove)
			{
				RemainingToRemove -= SlotQty;
				Items[SlotIdx] = FItemInfo();

				UpdateUI(SlotIdx, FItemInfo(), true);
			}
			else
			{
				Items[SlotIdx].ItemQuantity = SlotQty - RemainingToRemove;
				UpdateUI(SlotIdx, Items[SlotIdx], false);
				RemainingToRemove = 0;
			}

			if (RemainingToRemove == 0) break;
		}
	}*/
}

TArray<int32> UItemContainer::GetIndexesOfItem(const int32 ItemID) const
{
	TArray<int32> IndexesFound;
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index].ItemID == ItemID && Items[Index].ItemQuantity > 0)
		{
			IndexesFound.Add(Index);
		}
	}
	return IndexesFound;
}

bool UItemContainer::UpdateItemQuantity(int32 Index, int32 NewQuantity)
{
	if (NewQuantity <= 0)
	{
		return RemoveItemAtIndex(Index);
	}
	Items[Index].ItemQuantity = NewQuantity;
	UpdateUI(Index, Items[Index], false);
	return false;
}

std::pair<bool, int32> UItemContainer::RemoveQuantity(int32 Index, int32 AmountToRemove)
{
	int32 LocalCurrentQuantity = Items[Index].ItemQuantity;
	int32 NewQuantity = LocalCurrentQuantity - AmountToRemove;

	bool bSuccess = (NewQuantity >= 0);

	if (bSuccess) {
		UpdateItemQuantity(Index, NewQuantity);
	}
	
	return {bSuccess, LocalCurrentQuantity};
}

void UItemContainer::HandleSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex)
{
}

void UItemContainer::ServerOnSlotDrop_Implementation(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex)
{
	HandleSlotDrop(FromContainer, FromIndex, DroppedIndex);
}

bool UItemContainer::IsSlotEmpty(int32 SlotIndex) const
{
	return Items[SlotIndex].ItemID == 0;
}

bool UItemContainer::HasItemsToStack(const FItemInfo& ItemInfo) const
{
	bool ExistsInInventory = false;
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index].ItemID == ItemInfo.ItemID && Items[Index].ItemQuantity < Items[Index].StackSize)
		{
			ExistsInInventory = true;
			break;
		}
	}
	return ExistsInInventory;
}




