#pragma once

#include "CoreMinimal.h"
#include "ARK/Items/ToolEnums.h"
#include "Engine/DataTable.h"
#include "ResourceStructure.generated.h"

USTRUCT(BlueprintType)
struct FResourceStructure : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResourceID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHarvestingToolType PreferedToolType;
};
