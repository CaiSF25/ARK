// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerController.h"

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (WidgetClass)
		{
			CreatedWidget = CreateWidget<UMainWidget>(this,WidgetClass);
			if (CreatedWidget)
			{
				CreatedWidget->AddToViewport();
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

void ASurvivalPlayerController::ToggleInventory_Implementation(bool bShow)
{
	if (CreatedWidget)
	{
		CreatedWidget->SetInventoryVisibility(bShow);
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
