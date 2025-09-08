// Fill out your copyright notice in the Description page of Project Settings.


#include "CeilingMaster.h"
#include "Components/BoxComponent.h"

ACeilingMaster::ACeilingMaster()
{
	BuildableInfo.TraceChannel = ECC_GameTraceChannel3;
	BuildableInfo.UseCustomOverlap = true;

	OverlapBox->SetRelativeScale3D(FVector(3.f, 3.f, 0.3f));

	Ceiling = CreateDefaultSubobject<UBoxComponent>(TEXT("Ceiling"));
	Ceiling->SetupAttachment(StaticMesh);
	Ceiling->SetBoxExtent(FVector(100, 100, 10));
	Ceiling->SetRelativeLocation(FVector(0, 300, 0));
	Ceiling->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Ceiling->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ceiling->SetCollisionObjectType(ECC_WorldDynamic);
	Ceiling->SetCollisionResponseToAllChannels(ECR_Ignore);
	Ceiling->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	Ceiling1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Ceiling1"));
	Ceiling1->SetupAttachment(StaticMesh);
	Ceiling1->SetBoxExtent(FVector(100, 100, 10));
	Ceiling1->SetRelativeLocation(FVector(0, -300, 0));
	Ceiling1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Ceiling1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ceiling1->SetCollisionObjectType(ECC_WorldDynamic);
	Ceiling1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Ceiling1->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	Ceiling2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Ceiling2"));
	Ceiling2->SetupAttachment(StaticMesh);
	Ceiling2->SetBoxExtent(FVector(100, 100, 10));
	Ceiling2->SetRelativeLocation(FVector(300, 0, 0));
	Ceiling2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Ceiling2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ceiling2->SetCollisionObjectType(ECC_WorldDynamic);
	Ceiling2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Ceiling2->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	Ceiling3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Ceiling3"));
	Ceiling3->SetupAttachment(StaticMesh);
	Ceiling3->SetBoxExtent(FVector(100, 100, 10));
	Ceiling3->SetRelativeLocation(FVector(-300, 0, 0));
	Ceiling3->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Ceiling3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Ceiling3->SetCollisionObjectType(ECC_WorldDynamic);
	Ceiling3->SetCollisionResponseToAllChannels(ECR_Ignore);
	Ceiling3->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	Wall = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall"));
	Wall->SetupAttachment(StaticMesh);
	Wall->SetBoxExtent(FVector(150, 10, 150));
	Wall->SetRelativeLocation(FVector(152, 0, 157));
	Wall->SetRelativeRotation(FRotator(0, -90, 0));
	Wall->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall->SetCollisionObjectType(ECC_WorldDynamic);
	Wall->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall1"));
	Wall1->SetupAttachment(StaticMesh);
	Wall1->SetBoxExtent(FVector(150, 10, 150));
	Wall1->SetRelativeLocation(FVector(-152, 0, 157));
	Wall1->SetRelativeRotation(FRotator(0, 90, 0));
	Wall1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall1->SetCollisionObjectType(ECC_WorldDynamic);
	Wall1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall1->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall2"));
	Wall2->SetupAttachment(StaticMesh);
	Wall2->SetBoxExtent(FVector(150, 10, 150));
	Wall2->SetRelativeLocation(FVector(0, -152, 157));
	Wall2->SetRelativeRotation(FRotator(0, 180, 0));
	Wall2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall2->SetCollisionObjectType(ECC_WorldDynamic);
	Wall2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall2->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall3"));
	Wall3->SetupAttachment(StaticMesh);
	Wall3->SetBoxExtent(FVector(150, 10, 150));
	Wall3->SetRelativeLocation(FVector(0, 148, 157));
	Wall3->SetRelativeRotation(FRotator(0, 0, 0));
	Wall3->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall3->SetCollisionObjectType(ECC_WorldDynamic);
	Wall3->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall3->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);
}

TArray<UBoxComponent*> ACeilingMaster::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Ceiling);
	FoundBoxes.Add(Ceiling1);
	FoundBoxes.Add(Ceiling2);
	FoundBoxes.Add(Ceiling3);
	FoundBoxes.Add(Wall);
	FoundBoxes.Add(Wall1);
	FoundBoxes.Add(Wall2);
	FoundBoxes.Add(Wall3);
	return FoundBoxes;
}
