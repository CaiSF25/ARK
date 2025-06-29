// Fill out your copyright notice in the Description page of Project Settings.


#include "Trees.h"

ATrees::ATrees()
{
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
}
