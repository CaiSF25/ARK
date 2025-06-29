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

private:
	bool IsSwinging = false;

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

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerOverlap(FVector SpherePos);

	UFUNCTION(BlueprintCallable)
	void HarvestFoliage(float Damage, AActor* Ref);

public:
	// 接口实现
	virtual void UseItemInterface_Implementation(ASurvivalCharacter* CharRef) override;

	virtual void NotifyInterface_Implementation() override;

	virtual void EndAnimation_Implementation() override;
};
