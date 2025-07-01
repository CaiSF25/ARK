// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HarvestingMaster.h"
#include "ARK/Interfaces/GroundItemInterface.h"
#include "GroundItemMaster.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API AGroundItemMaster : public AHarvestingMaster , public IGroundItemInterface
{
	GENERATED_BODY()
	
public:
	AGroundItemMaster();

public:
	float GetHealth() const { return Health; }

	void SetHealth(const float LocalHealth) { Health = LocalHealth; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	float Health = 50.0f;

public:
	// 接口实现
	virtual AGroundItemMaster* GetGroundItemRef_Implementation() override;
};
