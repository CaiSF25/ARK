// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainer.h"

#include "ARK/Character/SurvivalCharacter.h"

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
	const int32 Index = FindEmptySlot();
	if (Index == -1) return false;
	Items[Index] = Item;
	UpdateUI(Index, Item, false);
	return true;
}

void UItemContainer::ServerAddItem_Implementation(const FItemInfo& Item)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Add Item");
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
	switch (ContainerType)
	{
	case EContainerType::PlayerInventory:
		ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOwner());
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

void UItemContainer::TransferItem(UItemContainer* ToComponent, const int32 ToSpecificIndex, const int32 ItemIndexToTransfer) const
{
	if (IsValid(ToComponent))
	{
		const FItemInfo ItemToTransfer = GetItemAtIndex(ItemIndexToTransfer);
		ToComponent->AddItemToIndex(ItemToTransfer, ToSpecificIndex, ItemIndexToTransfer);
	}
}

FItemInfo UItemContainer::GetItemAtIndex(const int32 Index) const
{
	return Items.IsValidIndex(Index) ? Items[Index] : FItemInfo();
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






