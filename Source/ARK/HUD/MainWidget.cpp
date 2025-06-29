// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "InventoryWidget.h"
#include "ItemAdded.h"
#include "ARK/Character/SurvivalPlayerController.h"
#include "Components/VerticalBox.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryWidget)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::SetInventoryVisibility(bool bVisible)
{
	if (bVisible)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::ShowItemCollected(UTexture2D* ResourceImage, int32 ResourceQuantity, FText ResourceName)
{
	UItemAdded* ItemAdded = CreateWidget<UItemAdded>(this, ItemAddedClass);
	ItemAdded->ResourceImage = ResourceImage;
	ItemAdded->ResourceQuantity = ResourceQuantity;
	ItemAdded->ResourceName = ResourceName;
	ResourceCollected->AddChild(ItemAdded);
}


