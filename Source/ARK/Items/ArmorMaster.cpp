// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmorMaster.h"

#include "ARK/Character/SurvivalCharacter.h"

AArmorMaster::AArmorMaster()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetOwnerNoSee(true);
}

void AArmorMaster::MasterPoseEvent_Implementation(ASurvivalCharacter* CharRef)
{
	if (CharRef && CharRef->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		if (const ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(CharRef)))
		{
			SkeletalMesh->SetLeaderPoseComponent(SurvivalCharacter->GetMesh3P());
		}
	}
}

AArmorMaster* AArmorMaster::GetArmorRef_Implementation()
{
	return this;
}
