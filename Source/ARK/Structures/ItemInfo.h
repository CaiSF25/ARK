#pragma once

#include "CoreMinimal.h"
#include "../Enums/ArkTypes.h"
#include "ARK/Items/ItemMaster.h"
#include "Engine/DataTable.h"
#include "ItemInfo.generated.h"

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCurHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsStackable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItemMaster> ItemClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType ArmorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo;
};
