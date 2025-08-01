// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ThirdPersonEquipable.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "Sound/SoundCue.h"
#include "TP_Hatchet.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API ATP_Hatchet : public AThirdPersonEquipable
{
	GENERATED_BODY()

public:
	ATP_Hatchet();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tool")
	EHarvestingToolType ToolType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tool")
	EToolTier ToolTier;

private:
	bool IsSwinging = false;

	UPROPERTY()
	ASurvivalCharacter* Character;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
	UAnimMontage* FirstPersonMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
	UAnimMontage* ThirdPersonMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataTable")
	UDataTable* LargeItemResourceTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UParticleSystem* PickaxeHitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	USoundAttenuation* DestructionAttenuation;
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientGetRotation();

	void OnOverlap(const FVector& SpherePos, const FRotator& Rotation);
	
	UFUNCTION(BlueprintCallable)
	void HarvestFoliage(const float Damage, AActor* Ref) const;

private:
	// 逻辑操作
	void Overlap(const FVector& SpherePos, const FRotator& Rotation);

	// ServerFunction
	UFUNCTION(Server, Reliable)
	void ServerOverlap(const FVector& SpherePos, const FRotator& Rotation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHitFX(FVector Location, FRotator Rotation);

public:
	// 接口实现
	virtual void UseItemInterface_Implementation(ASurvivalCharacter* CharRef) override;

	virtual void NotifyInterface_Implementation() override;

	virtual void EndAnimation_Implementation() override;
};
