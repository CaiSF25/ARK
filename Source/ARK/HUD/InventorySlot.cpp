// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"
#include "Input/Reply.h"
#include "ItemDrag.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	TopText->SetVisibility(ESlateVisibility::Hidden);
	ButtonText->SetVisibility(ESlateVisibility::Hidden);
	ItemHP->SetVisibility(ESlateVisibility::Hidden);
	QuantityText->SetVisibility(ESlateVisibility::Hidden);
}

FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	if (HasItemInSlot)
	{
		if (ItemInfo.ItemQuantity <= 0 || !DraggedItem)
		{
			OutOperation = nullptr;
			return;
		}

		UDraggedItem* DraggedWidget = CreateWidget<UDraggedItem>(GetOwningPlayer(), DraggedItem);
		if (!DraggedWidget)
		{
			OutOperation = nullptr;
			return;
		}
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "UInventorySlot::NativeOnMouseButtonDown");
		DraggedWidget->ImageIcon = ItemInfo.ItemIcon;
		DraggedWidget->TextTop = FText::AsNumber(ItemInfo.ItemDamage);
		DraggedWidget->ItemType = ItemInfo.ItemType;
		DraggedWidget->Quantity = FText::AsNumber(ItemInfo.ItemQuantity);
		DraggedWidget->UseAmmo = ItemInfo.UseAmmo;
		DraggedWidget->CurAmmo = ItemInfo.CurAmmo;
		DraggedWidget->MaxAmmo = ItemInfo.MaxAmmo;
		DraggedWidget->CurHP = ItemInfo.ItemQuantity;
		DraggedWidget->MaxHP = ItemInfo.ItemQuantity;

		UDragDropOperation* DragOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDrag::StaticClass());
		UItemDrag* ItemOp = Cast<UItemDrag>(DragOperation);
		if (!ItemOp)
		{
			OutOperation = nullptr;
			return;
		}
		ItemOp->DefaultDragVisual = DraggedWidget;
		ItemOp->SlotIndex = ItemIndex;
		ItemOp->FromContainer = ContainerType;
		ItemOp->ItemType = ItemInfo.ItemType;
		ItemOp->ArmorType = ItemInfo.ArmorType;
		OutOperation = DragOperation;
	}
	
	
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (const UItemDrag* ItemDrag = Cast<UItemDrag>(InOperation))
	{
		const int32 LocalSlotIndex = ItemDrag->SlotIndex;
		const EContainerType LocalContainerType = ItemDrag->FromContainer;
		const EArmorType ArmorType = ItemDrag->ArmorType;
		
		if (ACharacter* BaseCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			if (BaseCharacter->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
			{
				if (ASurvivalCharacter* SurvivalCharacter = ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(BaseCharacter))
				{
					SurvivalCharacter->OnSlotDrop(LocalContainerType, ContainerType, LocalSlotIndex, ItemIndex, ArmorType);
					return true;
				}
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventorySlot::UpdateSlot(const FItemInfo& LocalItemInfo)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Update UItem Container11");
	ItemInfo = LocalItemInfo;

	HasItemInSlot = true;

	ItemIcon->SetBrushFromTexture(ItemInfo.ItemIcon);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);

	FText FormattedText = FText::Format(
		FText::FromString("x{0}"),
		ItemInfo.ItemQuantity
		);
	QuantityText->SetText(FormattedText);
	QuantityText->SetVisibility(ESlateVisibility::Visible);
}

void UInventorySlot::ResetSlot()
{
	ButtonText->SetVisibility(ESlateVisibility::Hidden);
	ItemHP->SetVisibility(ESlateVisibility::Hidden);
	ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	QuantityText->SetVisibility(ESlateVisibility::Hidden);
	TopText->SetVisibility(ESlateVisibility::Hidden);

	HasItemInSlot = false;
}
