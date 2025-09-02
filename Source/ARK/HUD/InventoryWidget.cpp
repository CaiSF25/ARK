// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "Components/Image.h"
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

void UInventoryWidget::UpdateArmorSlot(const EArmorType& ArmorType, const FItemInfo& ItemInfo)
{
	UInventorySlot*& ArmorSlot = GetArmorSlotRef(ArmorType);
	ArmorSlot->UpdateSlot(ItemInfo);
}

void UInventoryWidget::RemoveArmorSlot(const EArmorType& ArmorType)
{
	UInventorySlot*& ArmorSlot = GetArmorSlotRef(ArmorType);
	ArmorSlot->ResetSlot();
}

void UInventoryWidget::SetPlayerTexture(UMaterialInterface* Mat) const
{
	PlayerImage->SetBrushFromMaterial(Mat);
}

UInventorySlot*& UInventoryWidget::GetArmorSlotRef(const EArmorType& ArmorType)
{
	switch (ArmorType)
	{
	case EArmorType::Helmet: return HelmetSlot;
	case EArmorType::Chest:  return ChestSlot;
	case EArmorType::Pants:  return PantsSlot;
	case EArmorType::Boots:  return BootsSlot;
	default:                 return HelmetSlot;
	}
}
