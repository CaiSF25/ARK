// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfoWindow.h"

#include "PlayerStat.h"
#include "ARK/Structures/PlayerStats.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UPlayerInfoWindow::UpdateStat(const EStatEnum& StatToUpdate, const float Current, const float Max) const
{
	const UPlayerStat* PlayerStats = nullptr;
	switch (StatToUpdate)
	{
	case EStatEnum::Health:
		PlayerStats = HealthPlayerStat;
		break;
	case EStatEnum::Food:
		PlayerStats = FoodPlayerStat;
		break;
	case EStatEnum::Water:
		PlayerStats = WaterPlayerStat;
		break;
	case EStatEnum::Stamina:
		PlayerStats = StaminaPlayerStat;
		break;
	}
	PlayerStats->UpdateStatWidget(Current, Max);
}

void UPlayerInfoWindow::UpdateExperience(int32 CurrentExperience, int32 MaxExperience) const
{
	if (ExperienceBar && MaxExperience > 0)
	{
		float ProgressPercent = static_cast<float>(CurrentExperience) / static_cast<float>(MaxExperience);
		ProgressPercent = FMath::Clamp(ProgressPercent, 0.0f, 1.0f);
		ExperienceBar->SetPercent(ProgressPercent);
	}
	else if (ExperienceBar)
	{
		ExperienceBar->SetPercent(0.0f);
	}

	if (ExperienceText)
	{
		ExperienceText->SetText(FText::FromString(FString::Printf(TEXT("XP.%d/%d"), CurrentExperience, MaxExperience)));
	}
}

void UPlayerInfoWindow::UpdateSkillPoints(const int32 SkillPoints) const
{
	if (IsValid(SkillPointsText))
	{
		SkillPointsText->SetText(FText::Format(
			NSLOCTEXT("SkillPoints", "SkillPointsFmt", "Skill Points: {0}"),
			FText::AsNumber(SkillPoints)
		));
	}
}
