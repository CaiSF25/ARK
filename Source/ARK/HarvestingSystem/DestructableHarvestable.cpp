// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableHarvestable.h"

#include "Net/UnrealNetwork.h"

// 重写
ADestructableHarvestable::ADestructableHarvestable()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

}

void ADestructableHarvestable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructableHarvestable::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADestructableHarvestable, Direction);
}

void ADestructableHarvestable::BeginPlay()
{
	Super::BeginPlay();
}

