// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemAdded.generated.h"

class USizeBox;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ARK_API UItemAdded : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource", meta = (ExposeOnSpawn))
	UTexture2D* ResourceImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource", meta = (ExposeOnSpawn))
	int32 ResourceQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource", meta = (ExposeOnSpawn))
	FText ResourceName;

protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "UI",meta=(BindWidget))
	USizeBox* ResourceWindow;

	UPROPERTY(EditAnywhere, Category = "UI",meta=(BindWidget))
	UImage* ResourceIcon;

	UPROPERTY(EditAnywhere, Category = "UI",meta=(BindWidget))
	UTextBlock* ResourceText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;

protected:
	UFUNCTION()
	void OnFadeOutFinished();
};
