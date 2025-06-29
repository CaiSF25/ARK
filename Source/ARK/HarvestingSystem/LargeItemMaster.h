// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HarvestingMaster.h"
#include "ARK/Interfaces/LargeItemInterface.h"
#include "LargeItemMaster.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API ALargeItemMaster : public AHarvestingMaster, public ILargeItemInterface
{
	GENERATED_BODY()

public:
	ALargeItemMaster();
	
public:
	float GetHealth() const { return Health; }

	void SetHealth(const float LocalHealth) { Health = LocalHealth; }

protected:
	float Health = 50.0;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	UStaticMeshComponent* StaticMesh;

public:
	// 接口实现
	virtual class ALargeItemMaster* GetLargeItemRef_Implementation() override;
};
