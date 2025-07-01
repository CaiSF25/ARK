// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundItemMaster.h"

AGroundItemMaster::AGroundItemMaster()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = SceneRoot;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(SceneRoot);
	StaticMesh->SetCollisionProfileName(TEXT("GroundItems"));
}

AGroundItemMaster* AGroundItemMaster::GetGroundItemRef_Implementation()
{
	return this;
}
