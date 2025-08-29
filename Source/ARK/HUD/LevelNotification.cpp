// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelNotification.h"

#include "Components/TextBlock.h"

void ULevelNotification::NativeConstruct()
{
	Super::NativeConstruct();

	if (TextPopAnimation)
	{
		PlayAnimation(TextPopAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}

	if (SkillPointsAnimation)
	{
		PlayAnimation(SkillPointsAnimation, 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);
	}

	if (IsValid(LevelUpText))
	{
		LevelUpText->SetText(FText::Format(
			NSLOCTEXT("PlayerLevel", "LevelUpFmt", "New Level Up! Level: {0}"),
			FText::AsNumber(PlayerLevel)
		));
	}
}

void ULevelNotification::SetPlayerLevel(int32 InPlayerLevel)
{
	PlayerLevel = InPlayerLevel;

	if (IsValid(LevelUpText))
	{
		LevelUpText->SetText(FText::Format(
			NSLOCTEXT("PlayerLevel", "LevelUpFmt", "New Level Up! Level: {0}"),
			FText::AsNumber(PlayerLevel)
		));
	}
}
