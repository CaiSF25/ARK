// Fill out your copyright notice in the Description page of Project Settings.


#include "DraggedItem.h"

void UDraggedItem::NativeConstruct()
{
	Super::NativeConstruct();

	ItemIcon->SetVisibility(ESlateVisibility::Visible);
	TopText->SetVisibility(ESlateVisibility::Hidden);
	ButtonText->SetVisibility(ESlateVisibility::Hidden);
	ItemHP->SetVisibility(ESlateVisibility::Hidden);
	QuantityText->SetVisibility(ESlateVisibility::Hidden);

	ItemIcon->SetBrushFromTexture(ImageIcon);
	switch (ItemType)
	{
	case EItemType::Resource:
		FText FormattedText = FText::Format(
		FText::FromString("x{0}"),
		Quantity
		);
		QuantityText->SetText(Quantity);
		QuantityText->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}

