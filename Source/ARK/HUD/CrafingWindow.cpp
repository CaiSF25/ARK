// Fill out your copyright notice in the Description page of Project Settings.


#include "CrafingWindow.h"

#include "CraftingStructs.h"
#include "ARK/Inventory/ItemInfo.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/MultiLineEditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "CraftingIngredient.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UCrafingWindow::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (MicsButton)
	{
		MicsButton->OnClicked.AddDynamic(this, &ThisClass::OnMicsButtonClicked);
	}

	if (ToolsButton)
	{
		ToolsButton->OnClicked.AddDynamic(this, &ThisClass::OnToolsButtonClicked);
	}

	if (ArmorButton)
	{
		ArmorButton->OnClicked.AddDynamic(this, &ThisClass::OnArmorButtonClicked);
	}

	if (StructuresButton)
	{
		StructuresButton->OnClicked.AddDynamic(this, &ThisClass::OnStructuresButtonClicked);
	}

	if (CraftItemButton)
	{
		CraftItemButton->OnClicked.AddDynamic(this, &ThisClass::OnCraftItemButtonClicked);
	}
}

void UCrafingWindow::OnMicsButtonClicked()
{
	CraftingGridSwitcher->SetActiveWidgetIndex(0);
}

void UCrafingWindow::OnToolsButtonClicked()
{
	CraftingGridSwitcher->SetActiveWidgetIndex(1);
}

void UCrafingWindow::OnArmorButtonClicked()
{
	CraftingGridSwitcher->SetActiveWidgetIndex(2);
}

void UCrafingWindow::OnStructuresButtonClicked()
{
	CraftingGridSwitcher->SetActiveWidgetIndex(3);
}

void UCrafingWindow::ShowItemRequirements(int32 ItemID)
{
	ItemSelectedID = ItemID;
	CraftRecipeInfo->SetActiveWidgetIndex(1);

	const FName RowName = FName(*FString::FromInt(ItemID));

	static const FString ContextString(TEXT("ItemRequirements"));

	if (!ItemRecipeDataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Could not found ItemRecipeDataTable");
	}
	
	FItemRecipe* Row = ItemRecipeDataTable->FindRow<FItemRecipe>(
		RowName,
		ContextString,
		true
		);
	
	if (Row)
	{
		SetupCraftingInfoTab(Row);

		const UWorld* World = GetWorld();
		if (!World) return;

		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
		if (PlayerCharacter && PlayerCharacter->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
		{
			ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(PlayerCharacter));
			Character->OnCheckIfCanCraftItem(ItemSelectedID, EContainerType::PlayerInventory, ECraftingType::PlayerInventory);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Row not found");
	}
}

void UCrafingWindow::UpdateCraftingStatus(const bool CanCraft) const
{
	if (CanCraft)
	{
		CanCraftBorder->SetBrushColor(FColor::Green);
		CraftItemButton->SetIsEnabled(true);
	}
	else
	{
		CanCraftBorder->SetBrushColor(FColor::Red);
		CraftItemButton->SetIsEnabled(false);
	}
}

void UCrafingWindow::OnCraftItemButtonClicked()
{
	const UWorld* World = GetWorld();
	if (!World) return;
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (Character && Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(Character));
		SurvivalCharacter->OnCraftItem(ItemSelectedID, EContainerType::PlayerInventory, ECraftingType::PlayerInventory);
	}
}

void UCrafingWindow::SetupCraftingInfoTab(FItemRecipe* ItemRecipe)
{
	ItemNameText->SetText(ItemRecipe->ItemName);
	ItemDescriptionText->SetText(ItemRecipe->ItemDesc);

	const FString RequiredLevelStr = FString::Printf(TEXT("Required Level: %d"),
		ItemRecipe->RequiredLevel
	);

	const FText RequiredLevel = FText::FromString(RequiredLevelStr);
	
	RequiredLevelText->SetText(RequiredLevel);

	ItemImage->SetBrushFromTexture(ItemRecipe->ItemIcon);

	RequiredItemsWidget->ClearChildren();

	for (int32 i = 0; i < ItemRecipe->RequiredItems.Num(); i++)
	{
		const FName RowName = FName(*FString::FromInt(ItemRecipe->RequiredItems[i].ItemID));
		static const FString ContextString(TEXT("RequiredItems"));

		if (!ItemInfoDataTable)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Could not found ItemInfoDataTable");
		}
		
		const FItemInfo* Row = ItemInfoDataTable->FindRow<FItemInfo>(
			RowName,
			ContextString,
			true
			);

		if (!WidgetClass)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "IngredientWidget not found");
		}
		
		UCraftingIngredient* IngredientWidget = CreateWidget<UCraftingIngredient>(this, WidgetClass);
		IngredientWidget->SetResourceText(Row->ItemName);
		IngredientWidget->SetResourceQuantity(ItemRecipe->RequiredItems[i].ItemQuantity);
		IngredientWidget->SetResourceImage(Row->ItemIcon);

		RequiredItemsWidget->AddChild(IngredientWidget);
	}
}
