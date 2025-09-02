// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ARK/Structures/EquipableInfo.h"
#include "PlayerWindowAnim.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UPlayerWindowAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess=true))
	class ASurvivalCharacter* SurvivalCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess=true))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category="Equipment", meta=(AllowPrivateAccess=true))
	EEquipableState EquippedItem = EEquipableState::Default;
};
