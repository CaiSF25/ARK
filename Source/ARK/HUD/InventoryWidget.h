// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrafingWindow.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainerGrid.h"
#include "Components/Button.h"
#include "InventoryWidget.generated.h"

class UPlayerInfoWindow;
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

	UFUNCTION(BlueprintCallable)
	UPlayerInfoWindow* GetPlayerInfoWindow() const { return PlayerInfoWindow;}
	
	UFUNCTION()
	void OnInventoryButtonClicked();

	UFUNCTION()
	void OnCraftingButtonClicked();

	UFUNCTION()
	void UpdateLevelText(int32 Level) const;

	// 护甲
	void UpdateArmorSlot(const EArmorType& ArmorType, const FItemInfo& ItemInfo);

	void RemoveArmorSlot(const EArmorType& ArmorType);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UItemContainerGrid* ItemContainerGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UCrafingWindow* CraftingWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UPlayerInfoWindow* PlayerInfoWindow;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* InventoryButton;

	UPROPERTY(meta=(BindWidget))
	UButton* CraftingButton;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* InventoryWidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerLevelText;

	UPROPERTY(meta=(BindWidget))
	UInventorySlot* HelmetSlot;

	UPROPERTY(meta=(BindWidget))
	UInventorySlot* ChestSlot;

	UPROPERTY(meta=(BindWidget))
	UInventorySlot* PantsSlot;

	UPROPERTY(meta=(BindWidget))
	UInventorySlot* BootsSlot;

	UInventorySlot*& GetArmorSlotRef(const EArmorType& ArmorType);
};
