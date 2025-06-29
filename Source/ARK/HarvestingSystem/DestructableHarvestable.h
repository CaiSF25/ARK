// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructableHarvestable.generated.h"

UCLASS()
class ARK_API ADestructableHarvestable : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestructableHarvestable();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn), Replicated)
	FVector Direction;
};
