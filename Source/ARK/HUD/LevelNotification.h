// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelNotification.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class ARK_API ULevelNotification : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerLevel(int32 InPlayerLevel);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn))
	int32 PlayerLevel = 1;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelUpText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillPointsText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TextPopAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* SkillPointsAnimation;
	
};
