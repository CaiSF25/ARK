// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingIngredient.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ARK_API UCraftingIngredient : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetResourceText(const FText& Text) { ResourceText = Text; }
	
	void SetResourceQuantity(const int32 Quantity) { ResourceQuantity = Quantity; }

	void SetResourceImage(UTexture2D* Icon) { ResourceIcon = Icon; }
	
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResourceName;

	UPROPERTY(meta = (BindWidget))
	UImage* ResourceImage;

	UPROPERTY()
	FText ResourceText;

	UPROPERTY()
	int32 ResourceQuantity;

	UPROPERTY()
	UTexture2D* ResourceIcon;
	
};
