// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainerGrid.h"

void UItemContainerGrid::NativeConstruct()
{
	Super::NativeConstruct();

	AddSlots(TotalSlots);
}

void UItemContainerGrid::AddSlots(int32 Amount)
{
	if (Amount < 0) return;
	APlayerController* PlayerController = GetOwningPlayer();
	for (int32 i = 0; i < Amount; i++)
	{
		UInventorySlot* NewSlot = CreateWidget<UInventorySlot>(PlayerController,WidgetClass);
		if (NewSlot)
		{
			Slots.Add(NewSlot);
		}
		AddSlotToGrid(i, NewSlot);
	}
}

void UItemContainerGrid::AddSlotToGrid(int32 Index, UInventorySlot* LocalSlots)
{
	if (!Grid || !LocalSlots || SlotsPerRow <= 0) return;

	const int32 Row = Index / SlotsPerRow;
	const int32 Column = Index % SlotsPerRow;

	Grid->AddChildToUniformGrid(LocalSlots, Row, Column);

}
