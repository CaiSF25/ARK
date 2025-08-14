// Fill out your copyright notice in the Description page of Project Settings.


#include "StateNotifycationWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UStateNotifycationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PlayAnimation(FadeinOutAnim, 0, 0);
	
	if (bIsStarvingOrDehydrated)
	{
		NotificationText->SetText(FText::FromString("You are starving! Eat some food!"));
		const FSlateColor TextColor(FLinearColor::Red);
		NotificationText->SetColorAndOpacity(TextColor);
		StateImage->SetBrushFromTexture(FoodIcon);
	}
	else
	{
		NotificationText->SetText(FText::FromString("You are dehydrated! Drink some water!"));
		const FSlateColor TextColor(FLinearColor::Blue);
		NotificationText->SetColorAndOpacity(TextColor);
		StateImage->SetBrushFromTexture(WaterIcon);
	}
}
