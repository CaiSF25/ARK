// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerStats.h"
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 库存系统
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

	// 制造系统
	void OnCraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);
	
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category="Input")
	EEquipableState EquipableState = EEquipableState::Default;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Input")
	int32 HorbarIndex;

	// 武器系统
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

	// 制造系统
	void OnCheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

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

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Attack();

	void Interact();

	// UI操作
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* UIInputContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	TArray<UInputAction*> HotbarActions;

	void HotbarPressed(int32 Index);
	
	// 数据表
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* GroundResourcesTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemsDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* PlayerItemRecipeDataTable;

	// 蒙太奇回调
	UFUNCTION()
	void OnThirdPersonMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnThirdPersonNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

	UFUNCTION()
	void OnPickUpMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void HandleAnyDamage(AActor* DamagedActor,
						 float Damage,
						 const UDamageType* DamageType,
						 AController* InstigatedBy,
						 AActor* DamageCauser);
	
	// 辅助函数
	ASurvivalPlayerController* ASurvivalCharacter::GetSurvivalController() const;

	UDataTable* GetRecipeDataTable(const ECraftingType& TableType) const;
	
	UItemContainer* GetContainer(const EContainerType& ContainerType) const;
	
	UFUNCTION()
	void ApplyDamageToPlayer(float Damage, AActor* DamageCauser);

	// 回调函数
	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	// 交互逻辑
	

	void SpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);
	
	// 入口函数
	void Hotbar(int32 Index);

	// 角色动作
	UFUNCTION(Server, Reliable)
	void ServerAttack();

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	// 库存系统
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerHotBar* PlayerHotBar;
	
	UFUNCTION(Server, Reliable)
	void ServerOnSlotDrop(
		EContainerType FromContainer,
		EContainerType TargetContainer,
		int32 FromIndex,
		int32 DroppedIndex,
		EArmorType ArmorType
		);

	void HandleSlotDrop(
		EContainerType FromContainer,
		EContainerType TargetContainer,
		int32 FromIndex,
		int32 DroppedIndex,
		EArmorType ArmorType
		);

	void UseHotbarFunction(int32 Index);
	
	void DequipCurItem(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerHotbar(int32 Index);
	
	// 武器系统
	UFUNCTION(Server, Reliable)
	void ServerDequipCurItem(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerSpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);
	
	// 采集系统
	bool bIsHarvesting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX", meta=(AllowPrivateAccess="true"))
	UParticleSystem* BushHarvestParticle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX", meta=(AllowPrivateAccess="true"))
	USoundCue* BushHarvestSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX", meta=(AllowPrivateAccess="true"))
	USoundAttenuation* BushHarvestAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PickUpMontage;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBush();

	FTimerHandle DelayHandle_CraftItem;
	
	UFUNCTION()
	void OnDelayFinished(FName& ItemIDToAdd, EContainerType& Container);
	
	void HarvestMontage();
	
	UFUNCTION(Server, Reliable)
	void ServerHarvestMontage();

	void OnHarvestMontage();

	void OverlapGroundItems();

	UFUNCTION(Server, Reliable)
	void ServerOverlapGroundItems();

	void OnOverlapGroundItems();

	void HarvestItem(FResourceStructure Resource);
	
	UFUNCTION(Server, Reliable)
	void ServerHarvestItem(FResourceStructure Resource);

	// 制造系统
	bool bIsCrafting = false;
	
	void AddCraftedItem(const FName& ItemIDToAdd, const EContainerType& Container);
	
	bool CheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);
	
	UFUNCTION(Server, Reliable)
	void ServerCheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

	std::tuple<FName, EContainerType, float> CraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);
	
	UFUNCTION(Server, Reliable)
	void ServerCraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);

	// 玩家状态
	FTimerHandle DecreaseStatsHandle;

	FTimerHandle DecreaseHealthHandle;
	
	bool bIsDead = false;
	
	UPROPERTY()
	FPlayerStats PlayerStats;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Starving)
	bool bIsStarving = false;

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Dehydrated)
	bool bIsDehydrated = false;

	void CheckStartHealthDrain();

	void CheckStopHealthDrain();

	UFUNCTION()
	void OnRep_Starving() const;

	UFUNCTION()
	void OnRep_Dehydrated() const;
	
	float DecreaseFloat(const float FloatToDecrease, const float Percentage, const float Max);
	
	void FoodWaterDrain();
	
	void PassiveStatDrain();
	
	void DecreaseStatsOverTime();
	
	UFUNCTION(Server, Reliable)
	void ServerDecreaseStatsOverTime();

	void OnDecreaseStatsOverTime();
	
	// 玩家状态健康值
	void DecreaseHealth();

	void RemoveHealth(float Amount);

	void DecreaseHealthOverTime();

	void OnDecreaseHealthOverTime();
	
	UFUNCTION(Server, Reliable)
	void ServerDecreaseHealthOverTime();
	
	// 玩家耐力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaDrainRate = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRate = 1.0f;
	
	UPROPERTY(Replicated)
	EStaminaState StaminaState = EStaminaState::Idle;

	void UpdateStaminaUI(float NewStamina);

	void ProcessStamina(float DeltaTime);

	void OnStartDrainStamina();
	
	void OnStopDrainStamina();

	UFUNCTION(Server, Reliable)
	void ServerStartDrainStamina();

	UFUNCTION(Server, Reliable)
	void ServerStopDrainStamina();


public:
	// 接口实现
	virtual AController* GetControllerFromChar_Implementation() override;

	virtual APawn* GetSurvivalCharRef_Implementation() override;
	
	virtual void ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage) override;

	virtual void FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage) override;

	virtual FVector GetArrowLocation_Implementation() override;

	virtual FRotator GetArrowRotation_Implementation() override;
};
