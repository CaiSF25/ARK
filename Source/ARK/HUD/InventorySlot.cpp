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
