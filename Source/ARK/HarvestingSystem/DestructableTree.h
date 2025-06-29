// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructableHarvestable.h"
#include "DestructableTree.generated.h"

class UCapsuleComponent;
/**
 * 
 */
UCLASS()
class ARK_API ADestructableTree : public ADestructableHarvestable
{
	GENERATED_BODY()
	
public:
	ADestructableTree();

protected:
	virtual void BeginPlay() override;

public:
	void AddForce() const;
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Root")
	USceneComponent* SceneRoot;
	
	UPROPERTY(Category="Collision", VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(Category="Collision", VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent1;

	UPROPERTY(Category="Collision", VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent2;

	UPROPERTY(Category="Collision", VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent3;

	UPROPERTY(Category="Collision", VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent4;

	UPROPERTY(Category=Mesh, VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

private:
	FTimerHandle DelayHandle;

	UFUNCTION()
	void OnDelayFinished();
};
