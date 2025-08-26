// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "Components/WidgetSwitcher.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryButton)
	{
		InventoryButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnInventoryButtonClicked);
	}

	if (CraftingButton)
	{
		CraftingButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnCraftingButtonClicked);
	}
}

void UInventoryWidget::OnInventoryButtonClicked()
{
	InventoryWidgetSwitcher->SetActiveWidgetIndex(0);
}

void UInventoryWidget::OnCraftingButtonClicked()
{
	InventoryWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UInventoryWidget::UpdateLevelText(const int32 Level) const
{
	PlayerLevelText->SetText(FText::AsNumber(Level));
}
