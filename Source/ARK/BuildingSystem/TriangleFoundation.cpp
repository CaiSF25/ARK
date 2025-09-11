// Fill out your copyright notice in the Description page of Project Settings.


#include "TriangleFoundation.h"
#include "Components/BoxComponent.h"

ATriangleFoundation::ATriangleFoundation()
{
	Tags.Add("Foundation");
	
	BuildableInfo.TraceChannel = ECC_GameTraceChannel5;
	BuildableInfo.UseCustomOverlap = true;
	BuildableInfo.CanPlaceGround = true;
	BuildableInfo.DoFloatCheck = true;

	OverlapBox->SetRelativeLocation(FVector(0, 7, 45));
	OverlapBox->SetRelativeScale3D(FVector(2, 1.1, 1));
	OverlapBox->SetBoxExtent(FVector(32, 32, 32));

	// 地基
	Foundation = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation"));
	Foundation->SetupAttachment(StaticMesh);
	Foundation->SetBoxExtent(FVector(150, 150, 85));
	Foundation->SetRelativeLocation(FVector(0, 237, 0));
	Foundation->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	Foundation->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Block);

	Foundation1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation1"));
	Foundation1->SetupAttachment(StaticMesh);
	Foundation1->SetBoxExtent(FVector(150, 150, 85));
	Foundation1->SetRelativeLocation(FVector(205, -118.4, 0));
	Foundation1->SetRelativeRotation(FRotator(0, 240, 0));
	Foundation1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation1->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation1->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	Foundation1->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Block);

	Foundation2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation2"));
	Foundation2->SetupAttachment(StaticMesh);
	Foundation2->SetBoxExtent(FVector(150, 150, 85));
	Foundation2->SetRelativeLocation(FVector(-205, -118.4, 0));
	Foundation2->SetRelativeRotation(FRotator(0, 120, 0));
	Foundation2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation2->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation2->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	Foundation2->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Block);

	// 三角地基
	TriangleFoundation = CreateDefaultSubobject<UBoxComponent>(TEXT("TriangleFoundation"));
	TriangleFoundation->SetupAttachment(StaticMesh);
	TriangleFoundation->SetBoxExtent(FVector(150, 150, 85));
	TriangleFoundation->SetRelativeLocation(FVector(0, 173, 0));
	TriangleFoundation->SetRelativeRotation(FRotator(0, 60, 0));
	TriangleFoundation->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TriangleFoundation->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriangleFoundation->SetCollisionObjectType(ECC_WorldDynamic);
	TriangleFoundation->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriangleFoundation->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECR_Block);

	TriangleFoundation1 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriangleFoundation1"));
	TriangleFoundation1->SetupAttachment(StaticMesh);
	TriangleFoundation1->SetBoxExtent(FVector(150, 150, 85));
	TriangleFoundation1->SetRelativeLocation(FVector(149.7, -87, 0));
	TriangleFoundation1->SetRelativeRotation(FRotator(0, 60, 0));
	TriangleFoundation1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TriangleFoundation1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriangleFoundation1->SetCollisionObjectType(ECC_WorldDynamic);
	TriangleFoundation1->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriangleFoundation1->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECR_Block);
	
	TriangleFoundation2 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriangleFoundation2"));
	TriangleFoundation2->SetupAttachment(StaticMesh);
	TriangleFoundation2->SetBoxExtent(FVector(150, 150, 85));
	TriangleFoundation2->SetRelativeLocation(FVector(-150, -87, 0));
	TriangleFoundation2->SetRelativeRotation(FRotator(0, 60, 0));
	TriangleFoundation2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	TriangleFoundation2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriangleFoundation2->SetCollisionObjectType(ECC_WorldDynamic);
	TriangleFoundation2->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriangleFoundation2->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECR_Block);

	// 墙
	Wall = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall"));
	Wall->SetupAttachment(StaticMesh);
	Wall->SetBoxExtent(FVector(150, 10, 150));
	Wall->SetRelativeLocation(FVector(0, 89, 235));
	Wall->SetRelativeRotation(FRotator(0, 0, 0));
	Wall->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall->SetCollisionObjectType(ECC_WorldDynamic);
	Wall->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall1"));
	Wall1->SetupAttachment(StaticMesh);
	Wall1->SetBoxExtent(FVector(150, 10, 150));
	Wall1->SetRelativeLocation(FVector(77, -45, 235));
	Wall1->SetRelativeRotation(FRotator(0, -120, 0));
	Wall1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall1->SetCollisionObjectType(ECC_WorldDynamic);
	Wall1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall1->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall2"));
	Wall2->SetupAttachment(StaticMesh);
	Wall2->SetBoxExtent(FVector(150, 10, 150));
	Wall2->SetRelativeLocation(FVector(-77, -45, 235));
	Wall2->SetRelativeRotation(FRotator(0, -240, 0));
	Wall2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall2->SetCollisionObjectType(ECC_WorldDynamic);
	Wall2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall2->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);
}

TArray<UBoxComponent*> ATriangleFoundation::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Foundation);
	FoundBoxes.Add(Foundation1);
	FoundBoxes.Add(Foundation2);
	FoundBoxes.Add(TriangleFoundation);
	FoundBoxes.Add(TriangleFoundation1);
	FoundBoxes.Add(TriangleFoundation2);
	FoundBoxes.Add(Wall);
	FoundBoxes.Add(Wall1);
	FoundBoxes.Add(Wall2);
	return FoundBoxes;
}
