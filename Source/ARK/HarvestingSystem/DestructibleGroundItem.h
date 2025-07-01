// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructableHarvestable.h"
#include "DestructibleGroundItem.generated.h"

class UGeometryCollectionComponent;
/**
 * 
 */
UCLASS()
class ARK_API ADestructibleGroundItem : public ADestructableHarvestable
{
	GENERATED_BODY()

public:
	ADestructibleGroundItem();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Root")
	USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	UGeometryCollectionComponent* GeometryCollectionComponent;

private:
	FTimerHandle DelayHandle;

	UFUNCTION()
	void OnDelayFinished();
};
