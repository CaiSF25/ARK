// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WallMaster.h"
#include "WindowFrame.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API AWindowFrame : public AWallMaster
{
	GENERATED_BODY()

public:
	AWindowFrame();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WindowBox;

public:
	// 接口实现
	virtual TArray<UBoxComponent*> GetBoxes_Implementation() override;
};
