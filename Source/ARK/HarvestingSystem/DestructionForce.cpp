// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructionForce.h"

#include "Components/SphereComponent.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemActor.h"
#include "PhysicsEngine/RadialForceComponent.h"

ADestructionForce::ADestructionForce()
{
	RadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("RadialFalloff"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	const float InitialRadius = SphereComponent->GetUnscaledSphereRadius();
	const float NewRadius = InitialRadius * 2.0f;
	SphereComponent->SetSphereRadius(NewRadius, true);
}

void ADestructionForce::BeginPlay()
{
	Super::BeginPlay();
	
	URadialFalloff* FieldNode = RadialFalloff->SetRadialFalloff(
		50000.0,
		0.0,
		1.0,
		0.0,
		SphereComponent->GetScaledSphereRadius(),
		SphereComponent->GetComponentLocation(),
		EFieldFalloffType::Field_FallOff_None
		);
	
	FieldSystemComponent->ApplyPhysicsField(
		true,
		EFieldPhysicsType::Field_ExternalClusterStrain,
		nullptr,
		FieldNode);
}
