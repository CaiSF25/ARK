// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperienceText.h"

#include "Components/TextBlock.h"

void UExperienceText::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ExperienceTextBlock))
	{
		ExperienceTextBlock->SetText(FText::Format(
			NSLOCTEXT("Experience", "AddedFmt", "Added +{0} Experience"),
			FText::AsNumber(Experience)
		));
	}

	if (ExperienceAnim)
	{
		FWidgetAnimationDynamicEvent FinishedDelegate;
		FinishedDelegate.BindDynamic(this, &UExperienceText::OnExperienceAnimFinished);

		BindToAnimationFinished(ExperienceAnim, FinishedDelegate);
		PlayAnimation(ExperienceAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
	else
	{
		RemoveFromParent();
	}
}

void UExperienceText::SetExperience(const int32 InExperience)
{
	Experience = InExperience;

	if (IsValid(ExperienceTextBlock))
	{
		ExperienceTextBlock->SetText(FText::Format(
			NSLOCTEXT("Experience", "AddedFmt", "Added +{0} Experience"),
			FText::AsNumber(Experience)
		));
	}
}

void UExperienceText::OnExperienceAnimFinished()
{
	RemoveFromParent();
}
