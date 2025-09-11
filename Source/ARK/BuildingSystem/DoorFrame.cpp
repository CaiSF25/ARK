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

	// 火炬
	TorchBox->SetRelativeLocation(FVector(-112, 10, 0));
	TorchBox1->SetRelativeRotation(FRotator(0, 0, 0));
	
	TorchBox1->SetRelativeLocation(FVector(112, 10, 0));
	TorchBox1->SetRelativeRotation(FRotator(0, 0, 0));
}

TArray<UBoxComponent*> ADoorFrame::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Ceiling);
	FoundBoxes.Add(Ceiling1);
	FoundBoxes.Add(DoorBox);
	FoundBoxes.Add(TriangleCeiling);
	FoundBoxes.Add(TriangleCeiling1);
	FoundBoxes.Add(TorchBox);
	FoundBoxes.Add(TorchBox1);
	return FoundBoxes;
}
