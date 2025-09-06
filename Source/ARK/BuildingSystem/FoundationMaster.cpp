// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundationMaster.h"

#include "MovieSceneTracksComponentTypes.h"
#include "Components/BoxComponent.h"

AFoundationMaster::AFoundationMaster()
{
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
}

TArray<UBoxComponent*> AFoundationMaster::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Foundation);
	FoundBoxes.Add(Foundation1);
	FoundBoxes.Add(Foundation2);
	FoundBoxes.Add(Foundation3);
	return FoundBoxes;
}
