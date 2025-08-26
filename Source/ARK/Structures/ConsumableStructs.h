#pragma once

#include "CoreMinimal.h"
#include "PlayerStats.h"
#include "Engine/DataTable.h"
#include "ConsumableStructs.generated.h"

enum class EStatEnum : uint8;

USTRUCT(BlueprintType)
struct FConsumable
{
	GENERATED_BODY()

	// 修改的属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	EStatEnum StatToModify = EStatEnum::Health;

	// 是否立即生效
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	bool bInstantEffect = true;

	// 恢复的数值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", 
			  meta = (EditCondition = "bInstantEffect", ClampMin = "0.0"))
	float RestoreAmount = 20.f;

	// 是否有持续效果
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	bool bOverTime = false;

	// 持续时间内每秒恢复的数值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", 
			  meta = (EditCondition = "bOverTime", ClampMin = "0.0"))
	float OverTimeAmount = 5.0f;

	// 持续时间（秒）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", 
			  meta = (EditCondition = "bOverTime", ClampMin = "0.0"))
	float OverTimeDuration = 5.0f;
};

USTRUCT(BlueprintType)
struct FConsumableInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Info")
	FText ItemName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable Info")
	TArray<FConsumable> StatsToUpdate = TArray<FConsumable>();
};