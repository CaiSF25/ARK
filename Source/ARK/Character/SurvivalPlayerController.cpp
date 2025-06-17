// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerController.h"

#include "ARK/HUD/InventoryWidget.h"

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (WidgetClass)
		{
			MainWidget = CreateWidget<UMainWidget>(this,WidgetClass);
			if (MainWidget)
			{
				MainWidget->AddToViewport();
			}
		}
	}
}

void ASurvivalPlayerController::HandleToggleInventory()
{
	if (IsLocalController())
	{
		ServerToggleInventory();
	}
}

void ASurvivalPlayerController::ServerUpdateItemSlot_Implementation(const EContainerType& Container, int32 Index,
	const FItemInfo& ItemInfo)
{
}

void ASurvivalPlayerController::UpdateItemSlot_Implementation(const EContainerType& Container, int32 Index,
                                                        const FItemInfo& ItemInfo)
{
	// CreatedWidget->InventoryWidget->ItemContainerGrid->CreatedWidgets
	if (MainWidget && MainWidget->GetInventoryWidget())
	{
		UItemContainerGrid* Grid = MainWidget->GetInventoryWidget()->GetItemContainerGrid();
		if (Grid && Grid->GetSlots().IsValidIndex(Index))
		{
			UInventorySlot* Slot = Grid->GetSlots()[Index];
			Slot->UpdateSlot(ItemInfo);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Update UItem Container11");
		}
	}
}

void ASurvivalPlayerController::ToggleInventory_Implementation(bool bShow)
{
	if (MainWidget)
	{
		MainWidget->SetInventoryVisibility(bShow);
	}

	bShowMouseCursor = bShow;
	if (bShow)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}


void ASurvivalPlayerController::ServerToggleInventory_Implementation()
{
	const bool bNextShow = !bIsInventoryVisible;
	bIsInventoryVisible = bNextShow;

	ToggleInventory(bNextShow);
}
