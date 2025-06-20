// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "InventoryWidget.h"

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


