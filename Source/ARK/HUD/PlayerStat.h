// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStat.generated.h"

enum class EStatEnum : uint8;
class UTextBlock;
class UProgressBar;
class UButton;
/**
 * 
 */
UCLASS()
class ARK_API UPlayerStat : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	FText GetDisplayText() { return StatName; };

	UFUNCTION()
	void UpdateStatWidget(const float Current, const float Max) const;
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* LevelStatButton;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess))
	FText StatName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess))
	EStatEnum Stat;
};
