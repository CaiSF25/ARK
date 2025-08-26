// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Hatchet.h"

#include "ARSessionConfig.h"
#include "ARK/HarvestingSystem/DestructableHarvestable.h"
#include "ARK/HarvestingSystem/LargeItemMaster.h"
#include "ARK/Interfaces/LargeItemInterface.h"
#include "ARK/Structures/LargeItem.h"
#include "ARK/Structures/ToolEnums.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ATP_Hatchet::ATP_Hatchet()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);

	LargeItemResourceTable = CreateDefaultSubobject<UDataTable>("LargeItemTable");
}

void ATP_Hatchet::ServerOverlap_Implementation(const FVector& SpherePos, const FRotator& Rotation)
{
	Overlap(SpherePos, Rotation);
}

void ATP_Hatchet::ClientGetRotation_Implementation()
{
	if (AActor* LocalOwner = GetOwner(); LocalOwner->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		OnOverlap(ISurvivalCharacterInterface::Execute_GetArrowLocation(LocalOwner), ISurvivalCharacterInterface::Execute_GetArrowRotation(LocalOwner));
	}
}

void ATP_Hatchet::OnOverlap(const FVector& SpherePos, const FRotator& Rotation)
{
	if (HasAuthority())
	{
		Overlap(SpherePos, Rotation);
	}
	else
	{
		ServerOverlap(SpherePos, Rotation);
	}
}

void ATP_Hatchet::HarvestFoliage(const float Damage, AActor* Ref) const
{
	if (Ref->GetClass()->ImplementsInterface(ULargeItemInterface::StaticClass()))
	{
		if (ALargeItemMaster* LargeItemMaster = ILargeItemInterface::Execute_GetLargeItemRef(Ref); IsValid(LargeItemMaster))
		{
			const float Health = LargeItemMaster->GetHealth() - Damage;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(Health));
			const FName RowName = FName(*LargeItemMaster->GetClass()->GetName());
			static const FString ContextString(TEXT("HandleTreeHit"));
			if (Health <= 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Tree Is Harvested");
				const FLargeItem* Row = LargeItemResourceTable->FindRow<FLargeItem>(
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
						FResourceStructure HarvestItem = Row->GivenItems[i];
						const float BaseVar = static_cast<float>(HarvestItem.Quantity);
						constexpr float RateVar = 1;
						const float ToolTypeVar = ToolType == HarvestItem.PreferedToolType ? FMath::RandRange(0.2, 0.4) : FMath::RandRange(0.01, 0.1);
						float ToolTierVar;
						const float DamageVar = Damage;
						switch (ToolTier)
						{
						case EToolTier::Stone:
							ToolTierVar = FMath::RandRange(0.8, 1.2);
							break;
						case EToolTier::Iron:
							ToolTierVar = FMath::RandRange(1.2, 1.6);
							break;
						}
						if (const int ResourceQuantity = FMath::RoundToFloat(BaseVar * RateVar * ToolTierVar * ToolTypeVar * DamageVar); ResourceQuantity > 0)
						{
							HarvestItem.Quantity = ResourceQuantity;
						
							if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
							{
								Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(Character))->OnHarvestItem(HarvestItem);
							}
						}
					} 
				}
			}
		}
	}
}

void ATP_Hatchet::Overlap(const FVector& SpherePos, const FRotator& Rotation)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

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
		for (int i = 0; i < OutActors.Num(); i++)
		{
			if (OutActors[i]->GetClass()->ImplementsInterface(ULargeItemInterface::StaticClass()))
			{
				HarvestFoliage(15, OutActors[i]);
				MulticastHitFX(SpherePos, Rotation);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "We Hit a Player");
				UGameplayStatics::ApplyDamage(OutActors[i], 15.0, Character->GetController(), this, UDamageType::StaticClass());
			}
			
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "No Hit");
	}

	UWorld* World = GetWorld();
	/*DrawDebugSphere(
		World,
		SpherePos,
		60.f,
		12,
		FColor::Red,
		false,
		10,
		0,
		2
		);*/
}

void ATP_Hatchet::MulticastHitFX_Implementation(FVector Location, FRotator Rotation)
{
	if (UWorld* World = GetWorld())
	{
		FHitResult HitResult;
		constexpr float TraceDistance = 100.f;
		const FVector ForwardVector = Rotation.Vector() * TraceDistance;
		const FVector EndLocation = Location + ForwardVector;
		
		World->LineTraceSingleByChannel(
			HitResult,
			Location,
			EndLocation,
			ECC_Visibility
			);

		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			PickaxeHitParticle,
			HitResult.Location
			);

		UGameplayStatics::PlaySoundAtLocation(
			World,
			HitSound,
			Location,
			FRotator(0, 0, 0),
			1,
			1,
			0,
			DestructionAttenuation
			);
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