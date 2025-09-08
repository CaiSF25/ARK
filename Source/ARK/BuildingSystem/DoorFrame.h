// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WallMaster.h"
#include "DoorFrame.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API ADoorFrame : public AWallMaster
{
	GENERATED_BODY()

public:
	ADoorFrame();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* DoorBox;

public:
	virtual TArray<UBoxComponent*> GetBoxes_Implementation() override;
};
