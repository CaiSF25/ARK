// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ARK/HUD/InventoryWidget.h"

class UEnhancedInputLocalPlayerSubsystem;

void ASurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(OpenUIAction, ETriggerEvent::Started, this, &ASurvivalPlayerController::HandleToggleInventory);
	}
}

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (UIInputContext)
			{
				Subsystem->AddMappingContext(UIInputContext, 5);
			}
		}
	}
	
	if (IsLocalController())
	{
		if (WidgetClass)
		{
			MainWidget = CreateWidget<UMainWidget>(this, WidgetClass);
			if (MainWidget)
			{
				MainWidget->AddToViewport();
			}
		}
	}
}

void ASurvivalPlayerController::ResetItemSlot_Implementation(const EContainerType& Container, int32 Index)
{
	UInventorySlot* Slot = GetInventoryWidget(Container, Index);
	if (IsValid(Slot))
	{
		Slot->ResetSlot();
	}
}

void ASurvivalPlayerController::HandleToggleInventory()
{
	if (IsLocalController())
	{
		ServerToggleInventory();
	}
}

UInventorySlot* ASurvivalPlayerController::GetInventoryWidget(EContainerType Container, int32 SlotIndex)
 {
	TArray<UInventorySlot*> Slots;
	switch (Container)
	{
	case EContainerType::PlayerInventory:
		Slots = MainWidget->GetInventoryWidget()->GetItemContainerGrid()->GetSlots();
		break;
	case EContainerType::PlayerHotbar:
		Slots = MainWidget->GetHotbar()->GetItemContainerGrid()->GetSlots();
		break;
	case EContainerType::PlayerStorage:
		break;
	case EContainerType::PlayerArmor:
		break;
	}
	return Slots[SlotIndex];
}

APlayerController* ASurvivalPlayerController::SurvivalGamePCRef_Implementation()
{
	return this;
}

void ASurvivalPlayerController::ShowItemWidget_Implementation(UTexture2D* ResourceImage, int32 ResourceQuantity,
                                                              const FText& ResourceName)
{
	MainWidget->ShowItemCollected(ResourceImage, ResourceQuantity, ResourceName);
}


void ASurvivalPlayerController::UpdateItemSlot_Implementation(const EContainerType& Container, int32 Index,
                                                              const FItemInfo& ItemInfo)
{
	// CreatedWidget->InventoryWidget->ItemContainerGrid->CreatedWidgets

	UInventorySlot* Slot = GetInventoryWidget(Container, Index);
	if (IsValid(Slot))
	{
		Slot->UpdateSlot(ItemInfo);
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
