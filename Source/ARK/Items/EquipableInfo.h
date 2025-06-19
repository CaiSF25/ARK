#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EquipableInfo.generated.h"

class AFirstPersonEquipable;

UENUM(BlueprintType)
enum class EquipableState : uint8
{
	Hatchet          UMETA(DisplayName = "Hatchet"),
	Bow              UMETA(DisplayName = "Bow"),
	Rock             UMETA(DisplayName = "Rock"),
	Rifle            UMETA(DisplayName = "Rifle"),
	RocketLauncher   UMETA(DisplayName = "RocketLauncher"),
	Default          UMETA(DisplayName = "Default"),
	Spear            UMETA(DisplayName = "Spear"),
	Swimming         UMETA(DisplayName = "Swimming"),
};

USTRUCT(BlueprintType)
struct FEquipableInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EquipableState AnimationState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AFirstPersonEquipable> FirstPersonEquipClass = nullptr;
};
