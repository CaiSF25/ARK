// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingContainerGrid.h"

#include "ARK/Inventory/PlayerInventory.h"
#include "ARK/Structures/CraftingStructs.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UCraftingContainerGrid::NativeConstruct()
{
	Super::NativeConstruct();

	AddSlots(CraftingType);
}

void UCraftingContainerGrid::AddSlots(const ECraftingType LocalCraftingType)
{
	UDataTable* DataTableRef = nullptr;
	switch (LocalCraftingType)
	{
	case ECraftingType::PlayerInventory:
		DataTableRef = PlayerItemRecipes;
		break;
	case ECraftingType::CookingPot:
		DataTableRef = PlayerItemRecipes;
		break;
	case ECraftingType::CraftingBench:
		DataTableRef = PlayerItemRecipes;
		break;
	case ECraftingType::Forge:
		DataTableRef = PlayerItemRecipes;
		break;
	case ECraftingType::AdvanceWorkBench:
		DataTableRef = PlayerItemRecipes;
		break;
	case ECraftingType::StorageBox:
		DataTableRef = PlayerItemRecipes;
		break;
	}

	if (!DataTableRef) return;

	static const FString Context(TEXT("Craft"));
	TArray<FItemRecipe*> AllRows;
	DataTableRef->GetAllRows(Context, AllRows);

	for (const FItemRecipe* Row : AllRows)
	{
		if (ContainerType == EContainerType::PlayerInventory &&
			Row->ItemCategory != ItemCategory)
		{
			continue;
		}
		UCraftingSlot* GridSlot = CreateWidget<UCraftingSlot>(GetOwningPlayer(), CraftingSlot);
		GridSlot->SetImageTexture(Row->ItemIcon);
		GridSlot->SetItemID(Row->ItemID);
		GridSlot->SetContainerType(ContainerType);
		Slots.Add(GridSlot);
		GridSlot->SetItemIndex(Slots.Num() - 1);
		AddSlotToGrid(Slots.Num() - 1, GridSlot);
	}
}

void UCraftingContainerGrid::AddSlotToGrid(const int32 Index, UCraftingSlot* SlotToAdd) const
{
	const int32 Row = Index / SlotsPerRow;
	const int32 Column = Index % SlotsPerRow;
	
	if (Grid && SlotToAdd)
	{
		if (UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(SlotToAdd))
		{
			GridSlot->SetRow(Row);
			GridSlot->SetColumn(Column);
		}
	}
}
