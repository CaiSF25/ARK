// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipableInfo.h"
#include "ItemMaster.h"
#include "ARK/Interfaces/EquipableItem.h"
#include "EquipableMaster.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API AEquipableMaster : public AItemMaster, public IEquipableItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equipable")
	FEquipableInfo EquipableInfo;
	
	virtual AEquipableMaster* GetEquipableRef_Implementation() override;
};
