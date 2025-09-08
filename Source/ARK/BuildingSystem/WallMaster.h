// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildableMaster.h"
#include "WallMaster.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API AWallMaster : public ABuildableMaster
{
	GENERATED_BODY()

public:
	AWallMaster();

protected:
	// 屋顶吸附
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Ceiling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Ceiling1;

public:
	// 接口实现
	virtual TArray<UBoxComponent*> GetBoxes_Implementation() override;
};
