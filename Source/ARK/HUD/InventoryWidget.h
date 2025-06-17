// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemContainerGrid.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	UItemContainerGrid* GetItemContainerGrid() const {return ItemContainerGrid; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UItemContainerGrid* ItemContainerGrid;
};
