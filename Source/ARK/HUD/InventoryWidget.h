// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrafingWindow.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainerGrid.h"
#include "Components/Button.h"
#include "InventoryWidget.generated.h"

class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class ARK_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintCallable)
	UItemContainerGrid* GetItemContainerGrid() const {return ItemContainerGrid; }

	UFUNCTION(BlueprintCallable)
	UCrafingWindow* GetCraftingWindow() const { return CraftingWindow; }
	
	UFUNCTION()
	void OnInventoryButtonClicked();

	UFUNCTION()
	void OnCraftingButtonClicked();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UItemContainerGrid* ItemContainerGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UCrafingWindow* CraftingWindow;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* InventoryButton;

	UPROPERTY(meta=(BindWidget))
	UButton* CraftingButton;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* InventoryWidgetSwitcher;
};
