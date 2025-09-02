// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ARK/Inventory/PlayerInventory.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "ARK/Inventory/PlayerHotBar.h"
#include "ARK/Structures/EquipableInfo.h"
#include "ARK/Structures/PlayerStats.h"
#include "Sound/SoundCue.h"
#include "SurvivalCharacter.generated.h"

class APlayerWindow;
class AArmorMaster;
struct FResourceStructure;
enum class ECraftingType : uint8;
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
	// 组件
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	
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

	void OnSpawnEquipableThirdPerson(TSubclassOf<AActor> Class, const FItemInfo& ItemInfo, int32 LocalEquippedIndex);

	// 制造系统
	void OnCraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Input")
	int32 HorbarIndex;

	// ------------------------------------------   装备系统   ------------------------------------------
	// 武器
	UPROPERTY(ReplicatedUsing=OnRep_EquipableState, EditAnywhere,BlueprintReadWrite, Category="Input")
	EEquipableState EquipableState = EEquipableState::Default;

	UFUNCTION()
	void OnRep_EquipableState();
	
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon, EditAnywhere,BlueprintReadWrite, Category="Weapon")
	AActor* ThirdPersonEquippedItem;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UPROPERTY(Replicated)
	FName ReplicatedEquipSocketName;

	UPROPERTY(Replicated)
	EEquipableState ReplicatedEquipState;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weapon")
	int32 EquippedIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Weapon")
	AActor* FirstPersonEquippedItem;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientSpawnEquipableFirstPerson(TSubclassOf<AActor> Class, FName SocketName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastWeaponEquip(AActor* Target, const FName& SocketName, const EEquipableState& EquippedState);
	
	UFUNCTION(NetMulticast, Reliable)
	void DequipThirdPerson();

	UFUNCTION(Client, Reliable)
	void DequipFirstPerson();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MontageMulticast(UAnimMontage* ThirdPersonMontage);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientMontage(UAnimMontage* FirstPersonMontage);

	UFUNCTION(Client, Reliable)
	void ClientEquipItem(const FEquipableInfo& Info);
	
	// 护甲
	UPROPERTY(ReplicatedUsing=OnRep_HelmetSlots)
	AItemMaster* HelmetSlots;

	UFUNCTION()
	void OnRep_HelmetSlots();

	UPROPERTY(ReplicatedUsing=OnRep_ChestSlots)
	AItemMaster* ChestSlots;

	UFUNCTION()
	void OnRep_ChestSlots();

	UPROPERTY(ReplicatedUsing=OnRep_PantsSlots)
	AItemMaster* PantsSlots;

	UFUNCTION()
	void OnRep_PantsSlots();

	UPROPERTY(ReplicatedUsing=OnRep_BootsSlots)
	AItemMaster* BootsSlots;

	UFUNCTION()
	void OnRep_BootsSlots();
	
	void OnEquipArmor(const EContainerType& FromContainer, int32 FromIndex, const EArmorType& ArmorTypeSlot);

	void OnDequipArmor(const EArmorType& ArmorSlot);
	
	// 采集系统
	void HarvestGroundItem(AActor* Ref);

	// 制造系统
	void OnCheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

	// 玩家经验与升级
	void OnApplySkillPoints(const EStatEnum& Stat);
	
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

	// ------------------------------------------   角色移动   ------------------------------------------
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Attack();

	void Interact();

	// 加速
	float SprintSpeed = 800.f;
	float WalkSpeed = 400.f;

	UPROPERTY()
	bool bLocalSprinting;
	
	UPROPERTY()
	float LocalStamina;

	UPROPERTY(Replicated) 
	bool bClientHasMoveInput = false;

	bool bLocalHasMoveInput = false;

	UFUNCTION(Server, Reliable)
	void ServerReportMoveInput(bool bHasInput);

	void StopSprinting();
	
	void UpdateMovementSpeed() const;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ConsumeableDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ExperienceDataTable;
	
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
	ASurvivalPlayerController* GetSurvivalController() const;

	static AArmorMaster* GetArmorMaster(AItemMaster* ItemMaster);

	UDataTable* GetRecipeDataTable(const ECraftingType& TableType) const;
	
	UItemContainer* GetContainer(const EContainerType& ContainerType) const;

	float GetStateToUpdate(const EStatEnum& State);

	float GetMaxState(const EStatEnum& State);

	APlayerWindow* GetPlayerWindow() const;
	
	UFUNCTION()
	void ApplyDamageToPlayer(float Damage, AActor* DamageCauser);

	// 回调函数
	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	// 交互逻辑
	void SpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);
	
	// 入口函数
	void Hotbar(const int32 Index);

	// 角色动作
	UFUNCTION(Server, Reliable)
	void ServerAttack();

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	// ------------------------------------------   库存系统   ------------------------------------------
	// 玩家库存
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerHotBar* PlayerHotBar;

	// 拖拽功能
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

	// Hotbar热键
	void UseHotbarFunction(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerHotbar(const int32 Index);

	// 装备武器
	void DequipCurItem(const int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerDequipCurItem(int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerSpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);

	// 装备护甲
	AItemMaster*& GetArmorSlotRefByType(EArmorType Type);
	
	UFUNCTION(Server, Reliable)
	void ServerEquipArmor(const EContainerType& FromContainer, const int32 FromIndex, const EArmorType& ArmorTypeSlot);

	void EquipArmor(const EContainerType& FromContainer, const int32 FromIndex, const EArmorType& ArmorTypeSlot);

	UFUNCTION(Server, Reliable)
	void ServerDequipArmor(const EArmorType& ArmorSlot);

	void DequipArmor(const EArmorType& ArmorSlot);
	
	void PostComp(AItemMaster* Target);

	bool bSetupSceneRenderDoOnce = false;
	
	UFUNCTION(Client, Reliable)
	void SetupSceneRender();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APlayerWindow> PlayerWindowClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor", meta = (AllowPrivateAccess = "true"))
	APlayerWindow* PlayerWindow;

	FTimerHandle SetupSceneRenderTimer;

	UFUNCTION(Client, Reliable)
	void ClientEquipArmor(const FItemInfo& ItemInfo);

	UFUNCTION(Client, Reliable)
	void ClientDequipArmor(const EArmorType& ArmorType);
	
	// 使用消耗品(立即生效）
	void ConsumeItem(int32 Index, const EContainerType& Container);

	void UpdateStatInstant(EStatEnum StatToChange, float Amount);

	// 使用消耗品(持续生效)
	FTimerHandle HealthHandle;

	FTimerHandle FoodHandle;

	FTimerHandle WaterHandle;

	int32 HealthTicksRemaining = 0;
	float HealthAmountPerTick = 1.0f;
	void HealthOverTime();

	int32 FoodTicksRemaining = 0;
	float FoodAmountPerTick = 1.0f;
	void FoodOverTime();

	int32 WaterTicksRemaining = 0;
	float WaterAmountPerTick = 1.0f;
	void WaterOverTime();
	
	void UpdateStatOverTime(EStatEnum StatToChange, float OverTimeAmount, float Duration);
	
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
	void OnDelayFinished(const FName& ItemIDToAdd, const EContainerType& Container) const;
	
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
	
	void AddCraftedItem(const FName& ItemIDToAdd, const EContainerType& Container) const;
	
	bool CheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);
	
	UFUNCTION(Server, Reliable)
	void ServerCheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType);

	std::tuple<FName, EContainerType, float, int32> CraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType) const;
	
	UFUNCTION(Server, Reliable)
	void ServerCraftItem(int32 ItemID, EContainerType Container, ECraftingType TableType);

	// 玩家状态
	FTimerHandle DecreaseStatsHandle;

	FTimerHandle DecreaseHealthHandle;
	
	bool bIsDead = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerStats)
	FPlayerStats PlayerStats;
	
	UPROPERTY(ReplicatedUsing=OnRep_Starving)
	bool bIsStarving = false;

	UPROPERTY(ReplicatedUsing=OnRep_Dehydrated)
	bool bIsDehydrated = false;
	
	void UpdateFoodUI() const;

	void UpdateWaterUI() const;

	void CheckStartHealthDrain();

	void CheckStopHealthDrain();

	UFUNCTION()
	void OnRep_Starving();

	UFUNCTION()
	void OnRep_Dehydrated();

	UFUNCTION()
	void OnRep_PlayerStats();
	
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
	
	void UpdateHealthUI() const;

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

	UPROPERTY(Replicated)
	bool bWantsToSprint = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsSprinting)
	bool bIsSprinting = false;

	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION(Server, Reliable)
	void ServerSetWantsToSprint(bool bNewWants);

	void OnSprintPressed();
	void OnSprintReleased();

	void EvaluateSprintState();

	void UpdateStaminaUI();

	UFUNCTION(Client, Reliable)
	void ClientUpdateStaminaUI();

	void ProcessStamina(float DeltaTime);

	void OnStartDrainStamina();
	
	void OnStopDrainStamina();

	UFUNCTION(Server, Reliable)
	void ServerStartDrainStamina();

	UFUNCTION(Server, Reliable)
	void ServerStopDrainStamina();

	// 玩家经验
	int32 GetExperienceLevel(int32 Level) const;
	
	void AddExperience(int32 Experience);

	UFUNCTION(Server, Reliable)
	void ServerAddExperience(int32 Experience);

	void OnAddExperience(int32 Experience);

	UFUNCTION(Server, Reliable)
	void ServerApplySkillPoints(const EStatEnum& Stat);

	void ApplySkillPoints(const EStatEnum& Stat);

public:
	// 接口实现
	virtual AController* GetControllerFromChar_Implementation() override;

	virtual APawn* GetSurvivalCharRef_Implementation() override;
	
	virtual void ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage) override;

	virtual void FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage) override;

	virtual FVector GetArrowLocation_Implementation() override;

	virtual FRotator GetArrowRotation_Implementation() override;

	virtual int32 GetTotalArmorPieces_Implementation() override;
};
