// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hotbar.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.h"
#include "MainWidget.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "UI")
	UHotbar* GetHotbar() const { return Hotbar; }
	
	UFUNCTION(BlueprintCallable)
	void SetInventoryVisibility(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void ShowItemCollected(UTexture2D* ResourceImage, int32 ResourceQuantity, FText ResourceName);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UItemAdded> ItemAddedClass;
	
private:
	UPROPERTY(EditAnywhere, Category = "UI",meta=(BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(EditAnywhere, Category = "UI",meta=(BindWidget))
	UHotbar* Hotbar;

	UPROPERTY(EditAnywhere, Category= "UI", meta=(BindWidget))
	UVerticalBox* ResourceCollected;
};
