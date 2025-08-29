// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStat.h"

#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerStat::NativeConstruct()
{
	Super::NativeConstruct();

	if (StatText)
	{
		StatText->TextDelegate.BindDynamic(this, &UPlayerStat::GetDisplayText);
	}

	if (LevelStatButton)
	{
		LevelStatButton->OnClicked.AddDynamic(this, &UPlayerStat::OnLevelStatButtonClicked);
	}
}
 
void UPlayerStat::UpdateStatWidget(const float Current, const float Max) const
{
	ProgressBar->SetPercent(Current / Max);
	StatText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), FMath::TruncToInt(Current), FMath::TruncToInt(Max))));
}

void UPlayerStat::OnLevelStatButtonClicked()
{
	if (GetOwningPlayerPawn() && GetOwningPlayerPawn()->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwningPlayerPawn()));
		if (Character)
		{
			Character->OnApplySkillPoints(Stat);
		}
	}
}
