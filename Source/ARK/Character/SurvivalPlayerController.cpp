// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ARK/HUD/InventoryWidget.h"
#include "ARK/HUD/PlayerInfoWindow.h"

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

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
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

void ASurvivalPlayerController::ShowOrHideDehydrated_Implementation(bool bShowOrHide)
{
	if (bShowOrHide)
	{
		MainWidget->ShowDehydratedNotification();
	}
	else
	{
		MainWidget->RemoveDehydratedNotification();
	}
}

void ASurvivalPlayerController::ShowOrHideStarving_Implementation(bool bShowOrHide)
{
	if (bShowOrHide)
	{
		MainWidget->ShowStarvingNotification();
	}
	else
	{
		MainWidget->RemoveStarvingNotification();
	}
}

void ASurvivalPlayerController::ResetItemSlot_Implementation(const EContainerType& Container, int32 Index)
{
	if (UInventorySlot* Slot = GetInventoryWidget(Container, Index); IsValid(Slot))
	{
		Slot->ResetSlot();
	}
}

void ASurvivalPlayerController::UpdateCraftStatus_Implementation(bool CanCraft)
{
	MainWidget->GetInventoryWidget()->GetCraftingWindow()->UpdateCraftingStatus(CanCraft);
}

void ASurvivalPlayerController::HandleToggleInventory()
{
	if (IsLocalController())
	{
		ServerToggleInventory();
	}
}

UInventorySlot* ASurvivalPlayerController::GetInventoryWidget(const EContainerType Container, const int32 SlotIndex) const
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

void ASurvivalPlayerController::UpdateStatBar_Implementation(const EStatEnum& State, const float Current, const float Max)
{
	MainWidget->GetPlayerStatsWindow()->UpdateBar(State, Current, Max);
	MainWidget->GetInventoryWidget()->GetPlayerInfoWindow()->UpdateStat(State, Current, Max);
}


void ASurvivalPlayerController::UpdateExperienceUI_Implementation(const int32 CurrentExp, const int32 MaxExp)
{
	MainWidget->GetInventoryWidget()->GetPlayerInfoWindow()->UpdateExperience(CurrentExp, MaxExp);
}

void ASurvivalPlayerController::ExperienceNotify_Implementation(const int32 Experience)
{
	MainWidget->AddExperienceNotification(Experience);
}

void ASurvivalPlayerController::LevelUpNotify_Implementation(const int32 Level)
{
	MainWidget->ShowLevelUpNotification(Level);
}

void ASurvivalPlayerController::UpdateLevelUp_Implementation(const int32 Level)
{
	MainWidget->GetInventoryWidget()->UpdateLevelText(Level);
}

void ASurvivalPlayerController::UpdateSkillPoints_Implementation(const int32 SkillPoints)
{
	MainWidget->GetInventoryWidget()->GetPlayerInfoWindow()->UpdateSkillPoints(SkillPoints);
}

void ASurvivalPlayerController::RemoveLevelNotify_Implementation()
{
	MainWidget->RemoveLevelUpNotification();
}

void ASurvivalPlayerController::UpdateArmorUI_Implementation(const EArmorType& ArmorType, const FItemInfo& ItemInfo)
{
	MainWidget->GetInventoryWidget()->UpdateArmorSlot(ArmorType, ItemInfo);
}

void ASurvivalPlayerController::RemoveArmorUI_Implementation(const EArmorType& ArmorType)
{
	MainWidget->GetInventoryWidget()->RemoveArmorSlot(ArmorType);
}

void ASurvivalPlayerController::SetRenderMaterial_Implementation(UMaterialInterface* Mat)
{
	MainWidget->GetInventoryWidget()->SetPlayerTexture(Mat);
}

AController* ASurvivalPlayerController::SurvivalGamePCRef_Implementation()
{
	return this;
}

void ASurvivalPlayerController::ShowCraftingProgressBar_Implementation(const float InTime)
{
	MainWidget->ShowCraftingBar(InTime);
}

void ASurvivalPlayerController::ShowItemWidget_Implementation(UTexture2D* ResourceImage, const int32 ResourceQuantity,
                                                              const FText& ResourceName)
{
	MainWidget->ShowItemCollected(ResourceImage, ResourceQuantity, ResourceName);
}


void ASurvivalPlayerController::UpdateItemSlot_Implementation(const EContainerType& Container, const int32 Index,
                                                              const FItemInfo& ItemInfo)
{
	// CreatedWidget->InventoryWidget->ItemContainerGrid->CreatedWidgets

	if (UInventorySlot* Slot = GetInventoryWidget(Container, Index); IsValid(Slot))
	{
		Slot->UpdateSlot(ItemInfo);
	}
}

void ASurvivalPlayerController::ToggleInventory_Implementation(const bool bShow)
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
		const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}


void ASurvivalPlayerController::ServerToggleInventory_Implementation()
{
	const bool bNextShow = !bIsInventoryVisible;
	bIsInventoryVisible = bNextShow;

	ToggleInventory(bNextShow);
}
