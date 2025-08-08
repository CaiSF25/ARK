// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UCraftingProgressBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetDurationTime(const float InTime) { DurationTime = InTime; };

protected:
	UFUNCTION()
	void OnCraftItemAnimationFinished();

private:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CraftItemAnimation;
	
	float DurationTime = 3.f;
};
