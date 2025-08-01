// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructableHarvestable.h"
#include "Sound/SoundCue.h"
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UParticleSystem* DestructionParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	USoundCue* DestructionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	USoundAttenuation* DestructionAttenuation;
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Root")
	USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	UGeometryCollectionComponent* GeometryCollectionComponent;
	
private:
	FTimerHandle DelayHandle;

	UFUNCTION()
	void OnDelayFinished();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFX();
};
