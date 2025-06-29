// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ARK/HarvestingSystem/ResourceStructure.h"
#include "ARK/Inventory/PlayerInventory.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "ARK/Inventory/PlayerHotBar.h"
#include "ARK/Items/EquipableInfo.h"
#include "SurvivalCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UArrowComponent;
class UAnimMontage;
class USoundBase;

UCLASS()
class ARK_API ASurvivalCharacter : public ACharacter, public ISurvivalCharacterInterface
{
	GENERATED_BODY()

public:
	ASurvivalCharacter();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	// 组件
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow1;

	// 角色移动
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FirstPersonInputContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* Interact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	// UI操作
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* UIInputContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	TArray<class UInputAction*> HotbarActions;

	// 库存
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerHotBar* PlayerHotBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTable;

protected:
	// 核心动作
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Attack();

	void InteractPressed();

	void HotbarPressed(int32 Index);

	// 蒙太奇回调
	UFUNCTION()
	void OnThirdPersonMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnThirdPersonNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

public:
	// 入口函数
	void OnSlotDrop(
		EContainerType FromContainer,
		EContainerType TargetContainer,
		int32 FromIndex,
		int32 DroppedIndex,
		EArmorType ArmorType
		);

	void OnHarvestItem(FResourceStructure Resource);

	void OnDequipCurItem(int32 Index);

	void OnSpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category="Input")
	EEquipableState EquipableState = EEquipableState::Default;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Input")
	int32 HorbarIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Input")
	AActor* ThirdPersonEquippedItem;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Input")
	int32 EquippedIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Input")
	AActor* FirstPersonEquippedItem;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void SpawnEquipableFirstPerson(TSubclassOf<AActor> Class, FName SocketName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastWeaponEquip(AActor* Target, FName SocketName, const EEquipableState& EquippedState);

	// 卸下武器
	UFUNCTION(NetMulticast, Reliable)
	void DequipThirdPerson();

	UFUNCTION(Client, Reliable)
	void DequipFirstPerson();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MontageMulticast(UAnimMontage* ThirdPersonMontage);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientMontage(UAnimMontage* FirstPersonMontage);

private:
	// 交互逻辑
	void HandleSlotDrop(
		EContainerType FromContainer,
		EContainerType TargetContainer,
		int32 FromIndex,
		int32 DroppedIndex,
		EArmorType ArmorType
		);

	void UseHotbarFunction(int32 Index);

	void HarvestItem(FResourceStructure Resource);
	
	void DequipCurItem(int32 Index);

	void SpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);

	// 入口函数
	void Hotbar(int32 Index);

	// Server Functions
	UFUNCTION(Server, Reliable)
	void ServerAttack();
	
	UFUNCTION(Server, Reliable)
	void ServerOnSlotDrop(
		EContainerType FromContainer,
		EContainerType TargetContainer,
		int32 FromIndex,
		int32 DroppedIndex,
		EArmorType ArmorType
		);

	UFUNCTION(Server, Reliable)
	void ServerHotbar(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerHarvestItem(FResourceStructure Resource);

	UFUNCTION(Server, Reliable)
	void ServerDequipCurItem(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerSpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);
	
public:
	// 接口实现
	virtual ASurvivalPlayerController* GetControllerFromChar_Implementation() override;

	virtual ASurvivalCharacter* GetSurvivalCharRef_Implementation() override;
	
	virtual void ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage) override;

	virtual void FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage) override;

	virtual FVector GetArrowLocation_Implementation() override;
};