// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Hotbar.generated.h"

class UItemContainerGrid;
/**
 * 
 */
UCLASS()
class ARK_API UHotbar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UItemContainerGrid* GetItemContainerGrid() const {return ItemContainerGrid; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UItemContainerGrid* ItemContainerGrid;
};
