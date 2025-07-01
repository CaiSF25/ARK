// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructableHarvestable.h"
#include "DestructableRock.generated.h"

class ADestructionForce;
class UGeometryCollectionComponent;
/**
 * 
 */
UCLASS()
class ARK_API ADestructableRock : public ADestructableHarvestable
{
	GENERATED_BODY()

public:
	ADestructableRock();

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
