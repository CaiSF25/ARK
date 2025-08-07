// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainer.h"

#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Character/SurvivalPlayerController.h"

UItemContainer::UItemContainer()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UItemContainer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UItemContainer::BeginPlay()
{
	Super::BeginPlay();

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
	
	/*for (int32 Index = 0; Index < Items.Num() && Remaining.ItemQuantity > 0; Index++)
	{
		if (Items[Index].ItemID == LocalItem.ItemID && Items[Index].ItemQuantity < Items[Index].StackSize)
		{
			const int32 SpaceAvailable = Items[Index].StackSize - Items[Index].ItemQuantity;
			const int32 AmountToAdd = FMath::Min(SpaceAvailable, Remaining.ItemQuantity);
			const int32 MaxStackSize = Items[Index].StackSize;
			const int32 TempSlotQuantity = Items[Index].ItemQuantity;
			int32 TotalItemQuantity = Item.ItemQuantity;
			const int32 CurSlotQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? MaxStackSize : TempSlotQuantity + TotalItemQuantity;
			LocalItem.ItemQuantity = CurSlotQuantity;
			Items[Index] = LocalItem;
			UpdateUI(Index, LocalItem, false);
			TotalItemQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? FMath::Clamp(TempSlotQuantity - (MaxStackSize - TempSlotQuantity), 0, TotalItemQuantity) : 0;
			LocalItem.ItemQuantity = TotalItemQuantity;
		}
	}
	
	if (Item.IsStackable)
	{
		if (HasItemsToStack(Item))
		{
			for (int Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].ItemID == LocalItem.ItemID && Items[Index].ItemQuantity < Items[Index].StackSize)
				{
					const int32 MaxStackSize = Items[Index].StackSize;
					const int32 TempSlotQuantity = Items[Index].ItemQuantity;
					int32 TotalItemQuantity = Item.ItemQuantity;
					const int32 CurSlotQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? MaxStackSize : TempSlotQuantity + TotalItemQuantity;
					LocalItem.ItemQuantity = CurSlotQuantity;
					Items[Index] = LocalItem;
					UpdateUI(Index, LocalItem, false);
					TotalItemQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? FMath::Clamp(TempSlotQuantity - (MaxStackSize - TempSlotQuantity), 0, TotalItemQuantity) : 0;
					LocalItem.ItemQuantity = TotalItemQuantity;
				}
			}
			if (LocalItem.ItemQuantity > 0)
			{
				if (Item.ItemQuantity > Item.StackSize)
				{
					int32 TotalItemQuantity = Item.ItemQuantity;
					const int32 MaxStackSize = Item.StackSize;
					while (TotalItemQuantity > 0)
					{
						if (const int32 EmptySlot = FindEmptySlot())
						{
							constexpr int32 TempSlotQuantity = 0;
							const int32 CurSlotQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? MaxStackSize : TempSlotQuantity + TotalItemQuantity;
							LocalItem.ItemQuantity = CurSlotQuantity;
							Items[EmptySlot] = LocalItem;
							UpdateUI(EmptySlot, LocalItem, false);
							TotalItemQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? FMath::Clamp(TempSlotQuantity - (MaxStackSize - TempSlotQuantity), 0, TotalItemQuantity) : 0;
						}
						else
						{
							return true;
						}
					}
					return true;
				}
				else
				{
					const int32 Index = FindEmptySlot();
					if (Index == -1) return false;
					Items[Index] = Item;
					UpdateUI(Index, Item, false);
					return true;
				}
			}
			else return true;
		}
		else
		{
			if (Item.ItemQuantity > Item.StackSize)
			{
				int32 TotalItemQuantity = Item.ItemQuantity;
				const int32 MaxStackSize = Item.StackSize;
				while (TotalItemQuantity > 0)
				{
					if (const int32 EmptySlot = FindEmptySlot())
					{
						constexpr int32 TempSlotQuantity = 0;
						const int32 CurSlotQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? MaxStackSize : TempSlotQuantity + TotalItemQuantity;
						LocalItem.ItemQuantity = CurSlotQuantity;
						Items[EmptySlot] = LocalItem;
						UpdateUI(EmptySlot, LocalItem, false);
						TotalItemQuantity = TempSlotQuantity + TotalItemQuantity >= MaxStackSize ? FMath::Clamp(TempSlotQuantity - (MaxStackSize - TempSlotQuantity), 0, TotalItemQuantity) : 0;
					}
					else
					{
						return true;
					}
				}
				return true;
			}
			else
			{
				const int32 Index = FindEmptySlot();
				if (Index == -1) return false;
				Items[Index] = Item;
				UpdateUI(Index, Item, false);
				return true;
			}
		}
		return true;
	}
	else
	{
		const int32 Index = FindEmptySlot();
		if (Index == -1) return false;
		Items[Index] = Item;
		UpdateUI(Index, Item, false);
		return true;
	}*/
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

bool UItemContainer::RemoveItem(int SlotIndex)
{
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

void UItemContainer::UpdateUI(int32 Index, const FItemInfo& Item, bool ResetSlot)
{
	ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOwner());
	switch (ContainerType)
	{
	case EContainerType::PlayerInventory:
		
		if (Character && Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
		{
			if (ASurvivalPlayerController* PlayerController = ISurvivalCharacterInterface::Execute_GetControllerFromChar(Character))
			{
				PlayerController->UpdateItemSlot(ContainerType, Index, Item);
			}
		}
		break;
	case EContainerType::PlayerHotbar:
		if (Character && Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
		{
			if (ASurvivalPlayerController* PlayerController = ISurvivalCharacterInterface::Execute_GetControllerFromChar(Character))
			{
				PlayerController->UpdateItemSlot(ContainerType, Index, Item);
			}
		}
		break;
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
	
	TArray<FItemStructure> HasItems = RequiredItems;
	for (int32 Index = 0; Index < RequiredItems.Num(); Index++)
	{
		for (int32 ItemIndex = 0; ItemIndex < Items.Num(); ItemIndex++)
		{
			if (RequiredItems[Index].ItemID == Items[ItemIndex].ItemID && RequiredItems[Index].ItemQuantity <= Items[ItemIndex].ItemQuantity)
			{
				for (int32 i = 0; i < HasItems.Num(); i++)
				{
					if (HasItems[i].ItemID ==  RequiredItems[Index].ItemID)
					{
						HasItems.RemoveAt(i);
					}
				}
			}
		}
	}
	if (HasItems.IsEmpty())
	{
		return true;
	}
	return false;
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




