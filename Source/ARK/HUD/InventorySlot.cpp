// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"

#include "IDetailTreeNode.h"
#include "Input/Reply.h"
#include "ItemDrag.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Structures/ItemInfo.h"
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

	if (IsValid(SlotStyle))
	{
		SlotStyle->SetBrushFromTexture(BackgroundImage);
	}
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
	const UItemDrag* ItemDrag = Cast<UItemDrag>(InOperation);
	if (!ItemDrag)
	{
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}
	
	const int32 LocalSlotIndex = ItemDrag->SlotIndex;
	const EContainerType LocalContainerType = ItemDrag->FromContainer;
	const EArmorType LocalArmorType = ItemDrag->ArmorType;

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] NativeOnDrop: GetWorld() == nullptr"), *GetName());
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerChar)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] NativeOnDrop: PlayerCharacter == nullptr"), *GetName());
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}

	if (!PlayerChar->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[%s] NativeOnDrop: Player does not implement SurvivalCharacterInterface"), *GetName());
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}

	ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(PlayerChar));
	if (!SurvivalCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] NativeOnDrop: Could not resolve ASurvivalCharacter ref"), *GetName());
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}

	if (LocalContainerType == ContainerType && LocalSlotIndex == ItemIndex)
	{
		// same slot, ignore
		return true; // 已处理（不需要其他处理）
	}

	if (bArmorSlot && ItemDrag->ItemType == EItemType::Armor)
	{
		if (ItemDrag->ArmorType == this->ArmorType)
		{
			SurvivalCharacter->OnEquipArmor(LocalContainerType, LocalSlotIndex, LocalArmorType);
			return true;
		}
	}
	else
	{
		SurvivalCharacter->OnSlotDrop(LocalContainerType, ContainerType, LocalSlotIndex, ItemIndex, LocalArmorType);
		return true;
	}
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventorySlot::UpdateSlot(const FItemInfo& LocalItemInfo)
{
	ItemInfo = LocalItemInfo;

	HasItemInSlot = true;

	ItemIcon->SetBrushFromTexture(ItemInfo.ItemIcon);
	ItemIcon->SetVisibility(ESlateVisibility::Visible);

	switch (ItemInfo.ItemType)
	{
	case EItemType::Resource:
		{
			const FText FormattedText = FText::Format(
		   FText::FromString("x{0}"),
		   ItemInfo.ItemQuantity
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
			  ItemInfo.ItemDamage
			  );
			TopText->SetText(FormattedText);
			TopText->SetVisibility(ESlateVisibility::Visible);
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
			ItemHP->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetPercent(StaticCast<float>(ItemInfo.ItemCurHp) / StaticCast<float>(ItemInfo.ItemMaxHp));
			if (ItemInfo.UseAmmo)
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
				 ItemInfo.ItemDamage
				 );
			TopText->SetText(FormattedText);
			TopText->SetVisibility(ESlateVisibility::Visible);
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
			ButtonText->SetVisibility(ESlateVisibility::Hidden);
			ItemHP->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetPercent(StaticCast<float>(ItemInfo.ItemCurHp) / StaticCast<float>(ItemInfo.ItemMaxHp));
			break;
		}
	case EItemType::Consumable:
		{
			const FText FormattedText = FText::Format(
			  FText::FromString("x{0}"),
			  ItemInfo.ItemQuantity
			  );
			QuantityText->SetText(FormattedText);
			QuantityText->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetVisibility(ESlateVisibility::Visible);
			ItemHP->SetPercent(StaticCast<float>(ItemInfo.ItemCurHp) / StaticCast<float>(ItemInfo.ItemMaxHp));
			ButtonText->SetVisibility(ESlateVisibility::Hidden);
			TopText->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	case EItemType::Buildable:
		{
			const FText FormattedText = FText::Format(
		   FText::FromString("x{0}"),
		   ItemInfo.ItemQuantity
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

void UInventorySlot::ResetSlot()
{
	ButtonText->SetVisibility(ESlateVisibility::Hidden);
	ItemHP->SetVisibility(ESlateVisibility::Hidden);
	ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	QuantityText->SetVisibility(ESlateVisibility::Hidden);
	TopText->SetVisibility(ESlateVisibility::Hidden);

	HasItemInSlot = false;
}

void UInventorySlot::SetBackgroundImage(UTexture2D* InBackgroundImage)
{
	BackgroundImage = InBackgroundImage;

	if (IsValid(SlotStyle))
	{
		SlotStyle->SetBrushFromTexture(BackgroundImage);
	}
}

void UInventorySlot::SetArmorType(const EArmorType& InArmorType)
{
	ArmorType = InArmorType;
}