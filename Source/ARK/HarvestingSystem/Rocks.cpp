// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocks.h"

ARocks::ARocks()
{
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
}
