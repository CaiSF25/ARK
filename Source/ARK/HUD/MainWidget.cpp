// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "InventoryWidget.h"
#include "ItemAdded.h"
#include "CraftingProgressBar.h"
#include "ExperienceText.h"
#include "LevelNotification.h"
#include "StateNotifycationWidget.h"
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

void UMainWidget::SetInventoryVisibility(const bool bVisible) const
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

void UMainWidget::ShowCraftingBar(const float InTime) const
{
	if (!CraftingProgressBarClass) return;
	UCraftingProgressBar* CraftingProgressBar = CreateWidget<UCraftingProgressBar>(GetOwningPlayer(), CraftingProgressBarClass);
	CraftingProgressBar->SetDurationTime(InTime);
	CraftWindowBox->AddChild(CraftingProgressBar);
}

void UMainWidget::ShowStarvingNotification()
{
	if (!IsValid(StarvingWidget))
	{
		if (!StateNotifycationWidgetClass) return;
		StarvingWidget = CreateWidget<UStateNotifycationWidget>(GetWorld(), StateNotifycationWidgetClass);
		StarvingWidget->bIsStarvingOrDehydrated = true;
		NotificationBox->AddChild(StarvingWidget);
	}
	else
	{
		NotificationBox->AddChild(StarvingWidget);
	}
}

void UMainWidget::ShowDehydratedNotification()
{
	if (!IsValid(DehydratedWidget))
	{
		if (!StateNotifycationWidgetClass) return;
		DehydratedWidget = CreateWidget<UStateNotifycationWidget>(GetWorld(), StateNotifycationWidgetClass);
		DehydratedWidget->bIsStarvingOrDehydrated = false;
		NotificationBox->AddChild(DehydratedWidget);
	}
	else
	{
		NotificationBox->AddChild(DehydratedWidget);
	}
}

void UMainWidget::RemoveStarvingNotification() const
{
	if (IsValid(StarvingWidget))
	{
		StarvingWidget->RemoveFromParent();
	}
}

void UMainWidget::RemoveDehydratedNotification() const
{
	if (IsValid(DehydratedWidget))
	{
		DehydratedWidget->RemoveFromParent();
	}
}

void UMainWidget::AddExperienceNotification(const int32 ExperienceAmount)
{
	if (!ExperienceTextClass) return;
	ExperienceText = CreateWidget<UExperienceText>(GetWorld(), ExperienceTextClass);
	if (ExperienceText)
	{
		ExperienceText->SetExperience(ExperienceAmount);
		NotificationBox->AddChild(ExperienceText);
	}
}

void UMainWidget::ShowLevelUpNotification(const int32 NewLevel)
{
	if (!LevelNotificationClass) return;
	if (IsValid(LevelNotification))
	{
		LevelNotification->RemoveFromParent();
	}
	LevelNotification = CreateWidget<ULevelNotification>(GetWorld(), LevelNotificationClass);
	if (LevelNotification)
	{
		LevelNotification->SetPlayerLevel(NewLevel);
		NotificationBox->AddChild(LevelNotification);
	}
}

void UMainWidget::RemoveLevelUpNotification() const
{
	if (IsValid(LevelNotification))
	{
		LevelNotification->RemoveFromParent();
	}
}


