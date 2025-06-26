#pragma once

#include "CoreMinimal.h"
#include "../Items/ItemMaster.h"
#include "Engine/DataTable.h"
#include "ItemInfo.generated.h"

class AItemMaster;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Resource      UMETA(DisplayName = "Resource"),
	Equipable     UMETA(DisplayName = "Equipable"),
	Armor         UMETA(DisplayName = "Armor"),
	Consumable    UMETA(DisplayName = "Consumable"),
	Buildable     UMETA(DisplayName = "Buildable"),
	None		  UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common        UMETA(DisplayName = "Common"),
	UnCommon      UMETA(DisplayName = "UnCommon"),
	Rare          UMETA(DisplayName = "Rare"),
	Epic          UMETA(DisplayName = "Epic"),
	Legendary     UMETA(DisplayName = "Legendary"),
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Helmet        UMETA(DisplayName = "Helmet"),
	Chest         UMETA(DisplayName = "Chest"),
	Arm           UMETA(DisplayName = "Arm"),
	Pants         UMETA(DisplayName = "Pants"),
	Boots         UMETA(DisplayName = "Boots"),
	None		  UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EContainerType : uint8
{
	PlayerInventory     UMETA(DisplayName = "PlayerInventory"),
	PlayerHotbar        UMETA(DisplayName = "PlayerHotbar"),
	PlayerStorage       UMETA(DisplayName = "PlayerStorage"),
	PlayerArmor         UMETA(DisplayName = "PlayerArmor"),
};

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName = FText::FromString("Unknown Item");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDesc = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity ItemRarity = EItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCurHp = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemMaxHp = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsStackable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackSize = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItemMaster> ItemClassRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType ArmorType = EArmorType::Helmet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseAmmo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo = 0;
};
