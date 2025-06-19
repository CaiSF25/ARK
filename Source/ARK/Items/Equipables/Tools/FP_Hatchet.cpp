// Fill out your copyright notice in the Description page of Project Settings.


#include "FP_Hatchet.h"

AFP_Hatchet::AFP_Hatchet()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
}
