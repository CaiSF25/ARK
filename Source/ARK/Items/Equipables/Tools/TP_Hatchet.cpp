// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Hatchet.h"

ATP_Hatchet::ATP_Hatchet()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);
}
