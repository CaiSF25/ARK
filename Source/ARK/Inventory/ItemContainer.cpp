// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemContainer.h"

UItemContainer::UItemContainer()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UItemContainer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UItemContainer::BeginPlay()
{
	Super::BeginPlay();
	
	
}




