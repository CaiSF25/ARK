// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemActor.h"
#include "DestructionForce.generated.h"

class USphereComponent;
class URadialFalloff;
/**
 * 
 */
UCLASS()
class ARK_API ADestructionForce : public AFieldSystemActor
{
	GENERATED_BODY()
public:
	ADestructionForce();

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URadialFalloff* RadialFalloff;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;
};
