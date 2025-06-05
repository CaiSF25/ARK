// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonAnimInstance.h"

#include "ARK/Character/SurvivalCharacter.h"

void UFirstPersonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	SurvivalCharacter = Cast<ASurvivalCharacter>(TryGetPawnOwner());
}

void UFirstPersonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (SurvivalCharacter == nullptr)
	{
		SurvivalCharacter = Cast<ASurvivalCharacter>(TryGetPawnOwner());
	}

	if (SurvivalCharacter == nullptr) return;

	FVector Velocity = SurvivalCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
}
