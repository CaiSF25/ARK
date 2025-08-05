#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CraftingStructs.generated.h"

UENUM(BlueprintType)
enum class ECraftingType : uint8
{
	PlayerInventory     UMETA(DisplayName = "PlayerInventory"),
	CookingPot          UMETA(DisplayName = "Iron"),
	CraftingBench       UMETA(DisplayName = "CraftingBench"),
	Forge               UMETA(DisplayName = "Forge"),
	AdvanceWorkBench    UMETA(DisplayName = "AdvanceWorkBench"),
	StorageBox          UMETA(DisplayName = "StorageBox")
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Mics                UMETA(DisplayName = "Mics"),
	Tools               UMETA(DisplayName = "Tools"),
	Armor               UMETA(DisplayName = "Armor"),
	Structures          UMETA(DisplayName = "Structures")
};

USTRUCT(BlueprintType)
struct FItemStructure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemQuantity;
};

USTRUCT(BlueprintType)
struct FItemRecipe : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDesc = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemStructure> RequiredItems = TArray<FItemStructure>();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemCategory ItemCategory = EItemCategory::Mics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemExperience = 0;
};
