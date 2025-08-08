// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingProgressBar.h"

#include "Animation/WidgetAnimation.h"

void UCraftingProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CraftingProgressBar");

	if (CraftItemAnimation && DurationTime > 0.f)
	{
		FWidgetAnimationDynamicEvent FinishedDelegate;
		FinishedDelegate.BindDynamic(this, &UCraftingProgressBar::OnCraftItemAnimationFinished);
		BindToAnimationFinished(CraftItemAnimation, FinishedDelegate);
		
		const float OriginalLength = CraftItemAnimation->GetEndTime();  

		const float PlaySpeed = OriginalLength / DurationTime;

		PlayAnimation(CraftItemAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, PlaySpeed, false);
	}
}

void UCraftingProgressBar::OnCraftItemAnimationFinished()
{
	RemoveFromParent();
}
