// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	TopText->SetVisibility(ESlateVisibility::Hidden);
	ButtonText->SetVisibility(ESlateVisibility::Hidden);
	ItemHP->SetVisibility(ESlateVisibility::Hidden);
	QuantityText->SetVisibility(ESlateVisibility::Hidden);
}

void UInventorySlot::UpdateSlot(const FItemInfo& ItemInfo)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Update UItem Container11");
	ItemIcon->SetBrushFromTexture(ItemInfo.ItemIcon);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);

	FText FormattedText = FText::Format(
		FText::FromString("x{0}"),
		ItemInfo.ItemQuantity
		);
	QuantityText->SetText(FormattedText);
	QuantityText->SetVisibility(ESlateVisibility::Visible);
}
