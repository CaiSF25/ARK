// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StateNotifycationWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ARK_API UStateNotifycationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	bool bIsStarvingOrDehydrated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	FText NotificationTextVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* FoodIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WaterIcon;

private:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeinOutAnim;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NotificationText;

	UPROPERTY(meta = (BindWidget))
	UImage* StateImage;
};
