// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainer.h"

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
	return true;
}

void UItemContainer::ServerAddItem_Implementation(const FItemInfo& Item)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Add Item");
	AddItem(Item);
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







