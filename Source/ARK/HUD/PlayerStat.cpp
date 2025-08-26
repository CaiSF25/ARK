// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStat.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerStat::NativeConstruct()
{
	Super::NativeConstruct();

	if (StatText)
	{
		StatText->TextDelegate.BindDynamic(this, &UPlayerStat::GetDisplayText);
	}
}
 
void UPlayerStat::UpdateStatWidget(const float Current, const float Max) const
{
	ProgressBar->SetPercent(Current / Max);
	StatText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), FMath::TruncToInt(Current), FMath::TruncToInt(Max))));
}
