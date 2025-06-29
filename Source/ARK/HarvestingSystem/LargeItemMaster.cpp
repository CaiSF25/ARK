// Fill out your copyright notice in the Description page of Project Settings.


#include "LargeItemMaster.h"

class ALargeItemMaster* ALargeItemMaster::GetLargeItemRef_Implementation()
{
	return this;
}

ALargeItemMaster::ALargeItemMaster()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);
}
