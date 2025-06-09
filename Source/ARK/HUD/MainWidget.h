// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI",meta=(BindWidget))
	class UInventoryWidget* InventoryWidget;

	UFUNCTION(BlueprintCallable)
	void SetInventoryVisibility(bool bVisible);
};
