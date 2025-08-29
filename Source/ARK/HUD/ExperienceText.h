// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExperienceText.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class ARK_API UExperienceText : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetExperience(int32 InExperience);

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 Experience = 0;
	
private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ExperienceTextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ExperienceAnim;

	UFUNCTION()
	void OnExperienceAnimFinished();
};
