// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundationMaster.h"

#include "MovieSceneTracksComponentTypes.h"
#include "Components/BoxComponent.h"

AFoundationMaster::AFoundationMaster()
{
	BuildableInfo.UseCustomOverlap = true;
	BuildableInfo.DoFloatCheck = true;
	
	BuildableInfo.TraceChannel = ECC_GameTraceChannel2;
	Foundation = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation"));
	Foundation->SetupAttachment(RootComponent);
	Foundation->SetBoxExtent(FVector(150, 150, 85));
	Foundation->SetRelativeLocation(FVector(0, 300, 0));
	Foundation->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	
	Foundation1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation1"));
	Foundation1->SetupAttachment(RootComponent);
	Foundation1->SetBoxExtent(FVector(150, 150, 85));
	Foundation1->SetRelativeLocation(FVector(0, -300, 0));
	Foundation1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation1->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation1->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	Foundation2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation2"));
	Foundation2->SetupAttachment(RootComponent);
	Foundation2->SetBoxExtent(FVector(150, 150, 85));
	Foundation2->SetRelativeLocation(FVector(300, 0, 0));
	Foundation2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation2->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation2->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	Foundation3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Foundation3"));
	Foundation3->SetupAttachment(RootComponent);
	Foundation3->SetBoxExtent(FVector(150, 150, 85));
	Foundation3->SetRelativeLocation(FVector(-300, 0, 0));
	Foundation3->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Foundation3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Foundation3->SetCollisionObjectType(ECC_WorldDynamic);
	Foundation3->SetCollisionResponseToAllChannels(ECR_Ignore);
	Foundation3->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	Wall = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall"));
	Wall->SetupAttachment(RootComponent);
	Wall->SetBoxExtent(FVector(150, 20, 150));
	Wall->SetRelativeLocation(FVector(0, 152, 235));
	Wall->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall->SetCollisionObjectType(ECC_WorldDynamic);
	Wall->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall1"));
	Wall1->SetupAttachment(RootComponent);
	Wall1->SetBoxExtent(FVector(150, 20, 150));
	Wall1->SetRelativeLocation(FVector(0, -152, 235));
	Wall1->SetRelativeRotation(FRotator(0, 180, 0));
	Wall1->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall1->SetCollisionObjectType(ECC_WorldDynamic);
	Wall1->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall1->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall2"));
	Wall2->SetupAttachment(RootComponent);
	Wall2->SetBoxExtent(FVector(150, 20, 150));
	Wall2->SetRelativeLocation(FVector(-152, 0, 235));
	Wall2->SetRelativeRotation(FRotator(0, 90, 0));
	Wall2->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall2->SetCollisionObjectType(ECC_WorldDynamic);
	Wall2->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall2->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);

	Wall3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall3"));
	Wall3->SetupAttachment(RootComponent);
	Wall3->SetBoxExtent(FVector(150, 20, 150));
	Wall3->SetRelativeLocation(FVector(152, 0, 235));
	Wall3->SetRelativeRotation(FRotator(0, -90, 0));
	Wall3->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	Wall3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Wall3->SetCollisionObjectType(ECC_WorldDynamic);
	Wall3->SetCollisionResponseToAllChannels(ECR_Ignore);
	Wall3 ->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Block);
}

TArray<UBoxComponent*> AFoundationMaster::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Foundation);
	FoundBoxes.Add(Foundation1);
	FoundBoxes.Add(Foundation2);
	FoundBoxes.Add(Foundation3);
	FoundBoxes.Add(Wall);
	FoundBoxes.Add(Wall1);
	FoundBoxes.Add(Wall2);
	FoundBoxes.Add(Wall3);
	return FoundBoxes;
}
