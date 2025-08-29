// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWindow.generated.h"

class UTextBlock;
class UProgressBar;
enum class EStatEnum : uint8;
class UPlayerStat;
/**
 * 
 */
UCLASS()
class ARK_API UPlayerInfoWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateStat(const EStatEnum& StatToUpdate, const float Current, const float Max) const;

	void UpdateExperience(int32 CurrentExperience, int32 MaxExperience) const;

	void UpdateSkillPoints(int32 SkillPoints) const;
	
private:
	UPROPERTY(meta = (BindWidget))
	UPlayerStat* HealthPlayerStat;

	UPROPERTY(meta = (BindWidget))
	UPlayerStat* FoodPlayerStat;

	UPROPERTY(meta = (BindWidget))
	UPlayerStat* WaterPlayerStat;

	UPROPERTY(meta = (BindWidget))
	UPlayerStat* StaminaPlayerStat;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExperienceBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExperienceText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillPointsText;
	
};
