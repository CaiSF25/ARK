#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PlayerStats.generated.h"

UENUM(BlueprintType)
enum class EStatEnum : uint8
{
	Health     UMETA(DisplayName = "Health"),
	Food       UMETA(DisplayName = "Food"),
	Water      UMETA(DisplayName = "Water"),
	Stamina    UMETA(DisplayName = "Stamina")
};

UENUM(BlueprintType)
enum class EStaminaState : uint8
{
	Idle,
	Draining,
	Regenerating
};

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentFood = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxFood = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentWater = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWater = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillPoints = 0;
	
};