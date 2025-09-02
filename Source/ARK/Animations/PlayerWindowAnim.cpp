// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWindowAnim.h"

#include "FirstPersonAnimInstance.h"
#include "ARK/Interfaces/PlayerWindowInterface.h"
#include "ARK/PlayerWindow/PlayerWindow.h"

void UPlayerWindowAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AActor* OwingActor = GetOwningActor();
	if (!IsValid(OwingActor)) return;
	if (OwingActor->GetClass()->ImplementsInterface(UPlayerWindowInterface::StaticClass()))
	{
		EquippedItem = IPlayerWindowInterface::Execute_GetPlayerWindowRef(OwingActor)->GetEquippedState();
	}
}
