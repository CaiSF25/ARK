// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ThirdPersonEquipable.h"
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
};
