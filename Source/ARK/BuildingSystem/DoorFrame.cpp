// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorFrame.h"
#include "Components/BoxComponent.h"

ADoorFrame::ADoorFrame()
{
	DoorBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorBox"));
	DoorBox->SetupAttachment(StaticMesh);
	DoorBox->SetBoxExtent(FVector(100, 10, 200));
	DoorBox->SetRelativeLocation(FVector(60, 0, -150));
	DoorBox->SetRelativeRotation(FRotator(0, 180, 0));
	DoorBox->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	DoorBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorBox->SetCollisionObjectType(ECC_WorldDynamic);
	DoorBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DoorBox->SetCollisionResponseToChannel(ECC_GameTraceChannel9, ECR_Block);
}

TArray<UBoxComponent*> ADoorFrame::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Ceiling);
	FoundBoxes.Add(Ceiling1);
	FoundBoxes.Add(DoorBox);
	return FoundBoxes;
}
