#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BuildableInfo.generated.h"

class ABuildableMaster;

USTRUCT(BlueprintType)
struct FBuildableInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	bool CanPlaceGround = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	bool UseCustomOverlap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	bool DoFloatCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	bool CanPlaceFoundation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	bool DoCellingCheck = false;
};

USTRUCT(BlueprintType)
struct FBuildTableInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BuildingSystem")
	TSubclassOf<ABuildableMaster> BuildClass;
};
