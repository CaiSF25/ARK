// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../FirstPersonEquipable.h"
#include "FP_Hatchet.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API AFP_Hatchet : public AFirstPersonEquipable
{
	GENERATED_BODY()

public:
	AFP_Hatchet();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh)
	UStaticMeshComponent* StaticMesh;
};
