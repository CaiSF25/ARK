// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsWindow.h"

#include "ARK/Structures/PlayerStats.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerStatsWindow::UpdateBar(const EStatEnum& State, const float Current, const float Max)
{
	UProgressBar* ProgressBar = nullptr;
	UTextBlock* TextBlock = nullptr;
	switch (State)
	{
	case EStatEnum::Health:
		ProgressBar = HealthBar;
		TextBlock = HealthText;
		break;
	case EStatEnum::Food:
		ProgressBar = FoodBar;
		TextBlock = FoodText;
		break;
	case EStatEnum::Water:
		ProgressBar = WaterBar;
		TextBlock = WaterText;
		break;
	case EStatEnum::Stamina:
		ProgressBar = StamBar;
		TextBlock = StamText;
		break;
	}
	if (ProgressBar != nullptr)
	{
		ProgressBar->SetPercent(Current / Max);
	}
	TextBlock->SetText(FText::AsNumber(FMath::TruncToInt(Current)));
}
