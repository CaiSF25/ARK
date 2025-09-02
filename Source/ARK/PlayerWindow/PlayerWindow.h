// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/Interfaces/PlayerWindowInterface.h"
#include "ARK/Structures/EquipableInfo.h"
#include "GameFramework/Actor.h"
#include "PlayerWindow.generated.h"

enum class EArmorType : uint8;
class AItemMaster;
struct FItemInfo;
class ASurvivalPlayerController;
class UCapsuleComponent;

UCLASS()
class ARK_API APlayerWindow : public AActor, public IPlayerWindowInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerWindow();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* Capsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USceneCaptureComponent2D* SceneCaptureComponent2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EEquipableState EquippedState = EEquipableState::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	AActor* EquippedItemRef;
	
	UPROPERTY()
	UMaterialInstanceDynamic* DynMat;
	
public:
	UFUNCTION(Client, Reliable)
	void InitialWindow(ASurvivalPlayerController* SurvivalPC);

	// 武器
	EEquipableState GetEquippedState() const {return EquippedState;}

	UFUNCTION()
	void EquipItem(const FEquipableInfo& Info);

	UFUNCTION()
	void DequipItem();

	// 护甲
	UPROPERTY()
	AItemMaster* HelmetSlot;

	UPROPERTY()
	AItemMaster* ChestSlot;

	UPROPERTY()
	AItemMaster* PantsSlot;

	UPROPERTY()
	AItemMaster* BootsSlot;
	
	void EquipArmor(const FItemInfo& ItemInfo);

	void DequipArmor(const EArmorType& ArmorType);
	
	// 接口实现
	virtual APlayerWindow* GetPlayerWindowRef_Implementation() override;
};
