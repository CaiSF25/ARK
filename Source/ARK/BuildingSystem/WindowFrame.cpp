// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowFrame.h"
#include "Components/BoxComponent.h"

AWindowFrame::AWindowFrame()
{
	WindowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WindowBox"));
	WindowBox->SetupAttachment(StaticMesh);
	WindowBox->SetBoxExtent(FVector(80, 20, 60));
	WindowBox->SetRelativeLocation(FVector(0, 0, 0));
	WindowBox->SetRelativeRotation(FRotator(0, 0, 0));
	WindowBox->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	WindowBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WindowBox->SetCollisionObjectType(ECC_WorldDynamic);
	WindowBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	WindowBox->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Block);
}

TArray<UBoxComponent*> AWindowFrame::GetBoxes_Implementation()
{
	TArray<UBoxComponent*> FoundBoxes;
	FoundBoxes.Add(Ceiling);
	FoundBoxes.Add(Ceiling1);
	FoundBoxes.Add(WindowBox);
	return FoundBoxes;
}
