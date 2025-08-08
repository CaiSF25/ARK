// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ARK/HarvestingSystem/ResourceStructure.h"
#include "ARK/HUD/CraftingStructs.h"
#include "ARK/Inventory/PlayerInventory.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "ARK/Inventory/PlayerHotBar.h"
#include "ARK/Items/EquipableInfo.h"
#include "Sound/SoundCue.h"
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
	UInputAction* InteractAction;

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

	// 数据表
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* GroundResourcesTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemsDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* PlayerItemRecipeDataTable;
	
protected:
	// 核心动作
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Attack();

	void Interact();

	void HotbarPressed(int32 Index);

	// 蒙太奇回调
	UFUNCTION()
	void OnThirdPersonMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnThirdPersonNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	UFUNCTION()
	void OnPickUpMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UParticleSystem* BushHarvestParticle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	USoundCue* BushHarvestSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	USoundAttenuation* BushHarvestAttenuation;
	
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

	void OnCheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

	void OnCraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);
	
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

	// 采集系统
	void HarvestGroundItem(AActor* Ref);

private:
	bool bIsHarvesting = false;

	bool bIsCrafting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PickUpMontage;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBush();

	FTimerHandle DelayHandle;

	UFUNCTION()
	void OnDelayFinished(FName& ItemIDToAdd, EContainerType& Container);

	void AddCraftedItem(const FName& ItemIDToAdd, const EContainerType& Container);

	UDataTable* GetRecipeDataTable(const ECraftingType& TableType) const;
	
	UItemContainer* GetContainer(const EContainerType& ContainerType) const;

	// 回调函数
	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
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

	void OverlapGroundItems();

	bool CheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

	std::tuple<FName, EContainerType, float> CraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);
	
	// 入口函数
	void Hotbar(int32 Index);

	void OnOverlapGroundItems();

	void OnHarvestMontage();

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

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	UFUNCTION(Server, Reliable)
	void ServerOverlapGroundItems();

	UFUNCTION(Server, Reliable)
	void ServerHarvestMontage();

	UFUNCTION(Server, Reliable)
	void ServerCheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

	UFUNCTION(Server, Reliable)
	void ServerCraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);

public:
	// 接口实现
	virtual AController* GetControllerFromChar_Implementation() override;

	virtual APawn* GetSurvivalCharRef_Implementation() override;
	
	virtual void ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage) override;

	virtual void FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage) override;

	virtual FVector GetArrowLocation_Implementation() override;

	virtual FRotator GetArrowRotation_Implementation() override;
};