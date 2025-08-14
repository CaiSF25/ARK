// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatsWindow.generated.h"

enum class EStatEnum : uint8;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class ARK_API UPlayerStatsWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateBar(const EStatEnum& State, const float Current, const float Max);

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* FoodBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* WaterBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StamBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FoodText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StamText;
	
};
