// Fill out your copyright notice in the Description page of Project Settings.


#include "RampMaster.h"

#include "Components/BoxComponent.h"

ARampMaster::ARampMaster()
{
	BuildableInfo.TraceChannel = ECC_GameTraceChannel15;
	BuildableInfo.UseCustomOverlap = true;

	OverlapBox->SetRelativeLocation(FVector(0, -70, 50));
	OverlapBox->SetRelativeScale3D(FVector(1.4f, 1, 1));
}
