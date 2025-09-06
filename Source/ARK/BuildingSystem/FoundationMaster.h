// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildableMaster.h"
#include "FoundationMaster.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API AFoundationMaster : public ABuildableMaster
{
	GENERATED_BODY()

public:
	AFoundationMaster();

	// 接口实现
	virtual TArray<UBoxComponent*> GetBoxes_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Foundation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Foundation1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Foundation2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Foundation3;
};
