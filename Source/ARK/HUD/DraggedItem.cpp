// Fill out your copyright notice in the Description page of Project Settings.


#include "DraggedItem.h"

#include "ARK/Structures/ItemInfo.h"

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

	switch (ItemType)
	{
	case EItemType::Resource:
		{
			const FText FormattedText = FText::Format(
		   FText::FromString("x{0}"),
		   Quantity
		   );
			QuantityText->SetText(FormattedText);
			QuantityText->SetVisibility(ESlateVisibility::Visible);
			TopText->SetVisibility(ESlateVisibility::Hidden);
			ButtonText->SetVisibility(ESlateVisibility::Hidden);
			ItemHP->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	case EItemType::Equipable:
		{
			const FText FormattedText = FText::Format(
			  FText::FromString("DMG {0}%"),
			  TextTop
			  );
			TopText->SetText(FormattedText);
			TopText->SetVisibility(ESlateVisibility::Visible);
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
			ItemHP->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetPercent(StaticCast<float>(CurHP) / StaticCast<float>(MaxHP));
			if (UseAmmo)
			{
				ButtonText->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				ButtonText->SetVisibility(ESlateVisibility::Hidden);
			}
			break;
		}
	case EItemType::Armor:
		{
			const FText FormattedText = FText::Format(
				 FText::FromString("{0} Armor"),
				 TextTop
				 );
			TopText->SetText(FormattedText);
			TopText->SetVisibility(ESlateVisibility::Visible);
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
			ButtonText->SetVisibility(ESlateVisibility::Hidden);
			ItemHP->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetPercent(StaticCast<float>(CurHP) / StaticCast<float>(MaxHP));
			break;
		}
	case EItemType::Consumable:
		{
			const FText FormattedText = FText::Format(
			  FText::FromString("x{0}"),
			  Quantity
			  );
			QuantityText->SetText(FormattedText);
			QuantityText->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetPercent(StaticCast<float>(CurHP) / StaticCast<float>(MaxHP));
			ButtonText->SetVisibility(ESlateVisibility::Hidden);
			TopText->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	case EItemType::Buildable:
		{
			const FText FormattedText = FText::Format(
		   FText::FromString("x{0}"),
		   Quantity
		   );
			QuantityText->SetText(FormattedText);
			QuantityText->SetVisibility(ESlateVisibility::Visible);
			TopText->SetVisibility(ESlateVisibility::Hidden);
			ButtonText->SetVisibility(ESlateVisibility::Hidden);
			ItemHP->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	default:
		break;
	}
}

