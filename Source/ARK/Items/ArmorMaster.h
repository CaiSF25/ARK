// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemMaster.h"
#include "ARK/Interfaces/ArmorItemInterface.h"
#include "ARK/Structures/ItemInfo.h"
#include "ArmorMaster.generated.h"

class ASurvivalCharacter;
struct FItemInfo;
/**
 * 
 */
UCLASS()
class ARK_API AArmorMaster : public AItemMaster, public IArmorItemInterface
{
	GENERATED_BODY()

public:
	AArmorMaster();
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MasterPoseEvent(ASurvivalCharacter* CharRef);
	
	// 接口实现
	virtual AArmorMaster* GetArmorRef_Implementation() override;

	FItemInfo GetItemInfo() const { return ItemInfo; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemInfo ItemInfo;
};
