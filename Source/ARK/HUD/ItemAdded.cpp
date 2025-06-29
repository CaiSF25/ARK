// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemAdded.h"

#include "MainWidget.h"

void UItemAdded::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeOut)
	{
		FWidgetAnimationDynamicEvent FinishedDelegate;
		FinishedDelegate.BindDynamic(this, &UItemAdded::OnFadeOutFinished);
		BindToAnimationFinished(FadeOut, FinishedDelegate);
		
		PlayAnimation(FadeOut);
	}
}

void UItemAdded::OnFadeOutFinished()
{
	RemoveFromParent();
}
