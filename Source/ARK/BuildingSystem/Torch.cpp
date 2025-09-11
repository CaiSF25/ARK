// Fill out your copyright notice in the Description page of Project Settings.


#include "Torch.h"

#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

ATorch::ATorch()
{
	BuildableInfo.TraceChannel = ECC_GameTraceChannel13;

	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>("FireParticle");
	FireParticle->SetupAttachment(Root);

	if (FireParticle)
	{
		FireParticle->SetRelativeLocation(FVector(-30, -45, 57));
		FireParticle->SetVisibility(false);
	}
}

void ATorch::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATorch, bIsTorchLit);
}

void ATorch::OnRep_IsTorchLit()
{
	FireParticle->SetVisibility(bIsTorchLit);
}

void ATorch::InteractEvent_Implementation(ASurvivalCharacter* CharRef)
{
	if (!HasAuthority()) return;
	bIsTorchLit = !bIsTorchLit;
	FireParticle->SetVisibility(bIsTorchLit);
}
