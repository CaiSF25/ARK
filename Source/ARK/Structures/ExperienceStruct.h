#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ExperienceStruct.generated.h"

USTRUCT(BlueprintType)
struct FExperienceStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ExperienceNeeded = 0;
};