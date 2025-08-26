#pragma once

#include "CoreMinimal.h"
#include "ToolEnums.generated.h"

UENUM(BlueprintType)
enum class EToolTier : uint8
{
	Stone     UMETA(DisplayName = "Stone"),
	Iron      UMETA(DisplayName = "Iron"),
};

UENUM(BlueprintType)
enum class EHarvestingToolType : uint8
{
	Hatchet   UMETA(DisplayName = "Hatchet"),
	Pickaxe   UMETA(DisplayName = "Pickaxe"),
};
