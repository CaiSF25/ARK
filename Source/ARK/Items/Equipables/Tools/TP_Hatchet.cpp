// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Hatchet.h"

#include "ARK/HarvestingSystem/DestructableHarvestable.h"
#include "ARK/HarvestingSystem/LargeItem.h"
#include "ARK/HarvestingSystem/LargeItemMaster.h"
#include "ARK/Interfaces/LargeItemInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ATP_Hatchet::ATP_Hatchet()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);

	LargeItemResourceTable = CreateDefaultSubobject<UDataTable>("LargeItemTable");
}

void ATP_Hatchet::ServerOverlap_Implementation(FVector SpherePos)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));

	const TSubclassOf<AActor> ClassFilter = nullptr;
	
	const TArray<AActor*> ActorsToIgnore = { Character };

	TArray<AActor*> OutActors;
	
	if (UKismetSystemLibrary::SphereOverlapActors(
		this,
		SpherePos,
		60.f,
		ObjectTypes,
		ClassFilter,
		ActorsToIgnore, 
		OutActors
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "We Hit a Tree");
		for (int i = 0; i < OutActors.Num(); i++)
		{
			HarvestFoliage(15, OutActors[i]);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "No Hit");
	}

	UWorld* World = GetWorld();
	DrawDebugSphere(
		World,
		SpherePos,
		60.f,
		12,
		FColor::Red,
		false,
		10,
		0,
		2
		);
}

void ATP_Hatchet::ClientGetRotation_Implementation()
{
	AActor* LocalOwner = GetOwner();
	if (LocalOwner->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		ServerOverlap(ISurvivalCharacterInterface::Execute_GetArrowLocation(LocalOwner));
	}
}

void ATP_Hatchet::HarvestFoliage(float Damage, AActor* Ref)
{
	if (Ref->GetClass()->ImplementsInterface(ULargeItemInterface::StaticClass()))
	{
		ALargeItemMaster* LargeItemMaster = ILargeItemInterface::Execute_GetLargeItemRef(Ref);
		if (IsValid(LargeItemMaster))
		{
			const float Health = LargeItemMaster->GetHealth() - Damage;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(Health));
			const FName RowName = FName(*LargeItemMaster->GetClass()->GetName());
			static const FString ContextString(TEXT("HandleTreeHit"));
			if (Health <= 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Tree Is Harvested");
				FLargeItem* Row = LargeItemResourceTable->FindRow<FLargeItem>(
					RowName,
					ContextString,
					true);
				const FTransform Transform = LargeItemMaster->GetActorTransform();
				LargeItemMaster->Destroy();
				if (UWorld* World = GetWorld())
				{
					ADestructableHarvestable* DestructableItem = World->SpawnActorDeferred<ADestructableHarvestable>(Row->Class, Transform);
					DestructableItem->Direction = Character->GetActorForwardVector();
					UGameplayStatics::FinishSpawningActor(DestructableItem, Transform);
				}
			}
			else
			{
				LargeItemMaster->SetHealth(Health);
				FLargeItem* Row = LargeItemResourceTable->FindRow<FLargeItem>(
					RowName,
					ContextString,
					true
					);
				if (Row)
				{
					for (int i = 0; i < Row->GivenItems.Num(); i++)
					{
						if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
						{
							ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(Character)->OnHarvestItem(Row->GivenItems[i]);
						}
					}
				}
			}
		}
	}
}

// 接口实现
void ATP_Hatchet::UseItemInterface_Implementation(ASurvivalCharacter* CharRef)
{
	if (!IsSwinging)
	{
		IsSwinging = true;
		Character = CharRef;
		if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
		{
			ISurvivalCharacterInterface::Execute_FirstPersonMontage(Character, FirstPersonMontage);
			ISurvivalCharacterInterface::Execute_ThirdPersonMontage(Character, ThirdPersonMontage);
		}
	}
}

void ATP_Hatchet::NotifyInterface_Implementation()
{
	ClientGetRotation();
}

void ATP_Hatchet::EndAnimation_Implementation()
{
	IsSwinging = false;
}