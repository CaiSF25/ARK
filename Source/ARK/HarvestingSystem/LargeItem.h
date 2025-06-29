#pragma once

#include "CoreMinimal.h"
#include "ResourceStructure.h"
#include "Engine/DataTable.h"
#include "LargeItem.generated.h"

USTRUCT(BlueprintType)
struct FLargeItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADestructableHarvestable> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FResourceStructure> GivenItems;
};