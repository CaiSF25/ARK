// Fill out your copyright notice in the Description page of Project Settings.


#include "WallMaster.h"

#include "Components/BoxComponent.h"

AWallMaster::AWallMaster()
{
	BuildableInfo.TraceChannel = ECC_GameTraceChannel14;

	// 屋顶
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

	// 三角屋顶
	TriangleCeiling = CreateDefaultSubobject<UBoxComponent>(TEXT("TriangleCeiling"));
	TriangleCeiling->SetupAttachment(StaticMesh);
	TriangleCeiling->SetBoxExtent(FVector(150, 150, 10));
	TriangleCeiling->SetRelativeLocation(FVector(0, -90, 152));
	TriangleCeiling->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TriangleCeiling->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriangleCeiling->SetCollisionObjectType(ECC_WorldDynamic);
	TriangleCeiling->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriangleCeiling->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Block);

	TriangleCeiling1 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriangleCeiling1"));
	TriangleCeiling1->SetupAttachment(StaticMesh);
	TriangleCeiling1->SetBoxExtent(FVector(150, 150, 10));
	TriangleCeiling1->SetRelativeLocation(FVector(0, 85, 152));
	TriangleCeiling1->SetRelativeRotation(FRotator(0, -60, 0));
	TriangleCeiling1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TriangleCeiling1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriangleCeiling1->SetCollisionObjectType(ECC_WorldDynamic);
	TriangleCeiling1->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriangleCeiling1->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Block);

	// 火炬
	TorchBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TorchBox"));
	TorchBox->SetupAttachment(StaticMesh);
	TorchBox->SetBoxExtent(FVector(60, 10, 60));
	TorchBox->SetRelativeLocation(FVector(0, 10, 0));
	TorchBox->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TorchBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TorchBox->SetCollisionObjectType(ECC_WorldDynamic);
	TorchBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TorchBox->SetCollisionResponseToChannel(ECC_GameTraceChannel13, ECR_Block);

	TorchBox1 = CreateDefaultSubobject<UBoxComponent>(TEXT("TorchBox1"));
	TorchBox1->SetupAttachment(StaticMesh);
	TorchBox1->SetBoxExtent(FVector(60, 10, 60));
	TorchBox1->SetRelativeLocation(FVector(0, -12, 0));
	TorchBox1->SetRelativeRotation(FRotator(0, -180, 0));
	TorchBox1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TorchBox1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TorchBox1->SetCollisionObjectType(ECC_WorldDynamic);
	TorchBox1->SetCollisionResponseToAllChannels(ECR_Ignore);
	TorchBox1->SetCollisionResponseToChannel(ECC_GameTraceChannel13, ECR_Block);
}

TArray<UBoxComponent*> AWallMaster::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Ceiling);
	FoundBoxes.Add(Ceiling1);
	FoundBoxes.Add(TriangleCeiling);
	FoundBoxes.Add(TriangleCeiling1);
	FoundBoxes.Add(TorchBox);
	FoundBoxes.Add(TorchBox1);
	return FoundBoxes;
}
