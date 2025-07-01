// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ThirdPersonEquipable.h"
#include "ARK/Character/SurvivalCharacter.h"
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
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientGetRotation();

	void OnOverlap(const FVector& SpherePos);
	
	UFUNCTION(BlueprintCallable)
	void HarvestFoliage(const float Damage, AActor* Ref) const;

private:
	// 逻辑操作
	void Overlap(const FVector& SpherePos);

	// ServerFunction
	UFUNCTION(Server, Reliable)
	void ServerOverlap(const FVector& SpherePos);

public:
	// 接口实现
	virtual void UseItemInterface_Implementation(ASurvivalCharacter* CharRef) override;

	virtual void NotifyInterface_Implementation() override;

	virtual void EndAnimation_Implementation() override;
};
