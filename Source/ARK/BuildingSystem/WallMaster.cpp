// Fill out your copyright notice in the Description page of Project Settings.


#include "WallMaster.h"

#include "Components/BoxComponent.h"

AWallMaster::AWallMaster()
{
	BuildableInfo.TraceChannel = ECC_GameTraceChannel14;

	Ceiling = CreateDefaultSubobject<UBoxComponent>(TEXT("Ceiling"));
	Ceiling->SetupAttachment(StaticMesh);
	Ceiling->SetBoxExtent(FVector(150, 150, 10));
	Ceiling->SetRelativeLocation(FVector(0, -152, 152));
	Ceiling->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Ceiling->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ceiling->SetCollisionObjectType(ECC_WorldDynamic);
	Ceiling->SetCollisionResponseToAllChannels(ECR_Ignore);
	Ceiling->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	Ceiling1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Ceiling1"));
	Ceiling1->SetupAttachment(StaticMesh);
	Ceiling1->SetBoxExtent(FVector(150, 150, 10));
	Ceiling1->SetRelativeLocation(FVector(0, 148, 152));
	Ceiling1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Ceiling1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ceiling1->SetCollisionObjectType(ECC_WorldDynamic);
	Ceiling1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Ceiling1->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

TArray<UBoxComponent*> AWallMaster::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Ceiling);
	FoundBoxes.Add(Ceiling1);
	return FoundBoxes;
}
