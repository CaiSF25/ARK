// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingStructs.h"
#include "Blueprint/UserWidget.h"
#include "CrafingWindow.generated.h"

class UBorder;
class UCraftingIngredient;
class UScrollBox;
class UImage;
class UMultiLineEditableText;
class UTextBlock;
class UDataTable;
class UWidgetSwitcher;
class UButton;
/**
 * 
 */
UCLASS()
class ARK_API UCrafingWindow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnMicsButtonClicked();

	UFUNCTION()
	void OnToolsButtonClicked();

	UFUNCTION()
	void OnArmorButtonClicked();

	UFUNCTION()
	void OnStructuresButtonClicked();

	UFUNCTION()
	void ShowItemRequirements(int32 ItemID);

	UFUNCTION()
	void UpdateCraftingStatus(const bool CanCraft) const;

	UFUNCTION()
	void OnCraftItemButtonClicked();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemRequirements")
	UDataTable* ItemRecipeDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemRequirements")
	UDataTable* ItemInfoDataTable;

	void SetupCraftingInfoTab(FItemRecipe* ItemRecipe);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCraftingIngredient> WidgetClass;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* MicsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ToolsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ArmorButton;

	UPROPERTY(meta = (BindWidget))
	UButton* StructuresButton;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* CraftingGridSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* CraftRecipeInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RequiredLevelText;

	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableText* ItemDescriptionText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Ingredients;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* RequiredItemsWidget;

	UPROPERTY(meta = (BindWidget))
	UBorder* CanCraftBorder;

	UPROPERTY(meta = (BindWidget))
	UButton* CraftItemButton;

	int32 ItemSelectedID;
};
