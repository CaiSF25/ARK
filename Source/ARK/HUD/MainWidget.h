// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hotbar.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.h"
#include "MainWidget.generated.h"

class UCraftingProgressBar;
class UVerticalBox;
class UItemAdded;
class USizeBox;
/**
 * 
 */
UCLASS()
class ARK_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

	UFUNCTION()
	UHotbar* GetHotbar() const { return Hotbar; }
	
	UFUNCTION()
	void SetInventoryVisibility(const bool bVisible) const;

	UFUNCTION(BlueprintCallable)
	void ShowItemCollected(UTexture2D* ResourceImage, int32 ResourceQuantity, FText ResourceName);

	UFUNCTION()
	void ShowCraftingBar(const float InTime) const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UItemAdded> ItemAddedClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCraftingProgressBar> CraftingProgressBarClass;
	
private:
	UPROPERTY(meta=(BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	UHotbar* Hotbar;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* ResourceCollected;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* CraftWindowBox;
};
