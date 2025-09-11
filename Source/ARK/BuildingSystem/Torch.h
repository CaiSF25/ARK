// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildableMaster.h"
#include "ARK/Interfaces/InteractableInterface.h"
#include "Torch.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API ATorch : public ABuildableMaster, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ATorch();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsTorchLit)
	bool bIsTorchLit = false;

	UFUNCTION()
	void OnRep_IsTorchLit();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* FireParticle;

public:
	// 接口实现
	virtual void InteractEvent_Implementation(ASurvivalCharacter* CharRef) override;
};
