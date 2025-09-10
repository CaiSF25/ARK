// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorMaster.h"

#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"

ADoorMaster::ADoorMaster()
{
	BuildableInfo.TraceChannel = ECC_GameTraceChannel9;

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
	DoorCurve = nullptr;

	bIsDoorOpen = false;
}

void ADoorMaster::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoorMaster, bIsDoorOpen);
}

void ADoorMaster::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetActorRotation().Yaw;
	bHasInitialYaw = true;

	if (DoorCurve && DoorTimeline)
	{
		FOnTimelineFloat ProgressFunc;
		ProgressFunc.BindUFunction(this, FName("TimelineProgress"));
		DoorTimeline->AddInterpFloat(DoorCurve, ProgressFunc);

		DoorTimeline->SetLooping(false);
		DoorTimeline->SetPlayRate(1.f);
	}
}

void ADoorMaster::OnRep_IsDoorOpen()
{
	if (!bHasInitialYaw)
	{
		InitialYaw = GetActorRotation().Yaw;
		bHasInitialYaw = true;
	}

	const float TargetYaw = InitialYaw + (bIsDoorOpen ? DoorOpenAngle : 0.f);
	FRotator R = GetActorRotation();
	R.Yaw = TargetYaw;
	SetActorRotation(R);
}

void ADoorMaster::TimelineProgress(float Value)
{
	float NewYaw = FMath::Lerp(InitialYaw, InitialYaw + DoorOpenAngle, Value);
	
	FRotator R = GetActorRotation();
	R.Yaw = NewYaw;
	SetActorRotation(R);
}

void ADoorMaster::MulticastSetDoorRotation_Implementation(const bool OpenOrClose)
{
	if (OpenOrClose)
	{
		DoorTimeline->PlayFromStart();
	}
	else
	{
		DoorTimeline->ReverseFromEnd();
	}
}

void ADoorMaster::InteractEvent_Implementation(ASurvivalCharacter* CharRef)
{
	if (!HasAuthority()) return;
	bIsDoorOpen = !bIsDoorOpen;
	MulticastSetDoorRotation(bIsDoorOpen);
}
