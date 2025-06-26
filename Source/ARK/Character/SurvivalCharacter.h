// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ARK/Inventory/PlayerInventory.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "ARK/Inventory/PlayerHotBar.h"
#include "ARK/Items/EquipableInfo.h"
#include "ARK/Items/Equipables/ThirdPersonEquipable.h"
#include "SurvivalCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
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
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar5Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar6Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar7Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar8Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar9Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	UInputAction* Hotbar10Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerInventory* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UPlayerHotBar* PlayerHotBar;

protected:
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Attack();

	void InteractPressed();

	void HotbarPressed(int32 Index);

	UFUNCTION()
	void OnThirdPersonMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnThirdPersonNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

public:
	virtual ASurvivalPlayerController* GetControllerFromChar_Implementation() override;

	virtual class ASurvivalCharacter* GetSurvivalCharRef_Implementation() override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerOnSlotDrop(
		EContainerType FromContainer,
		EContainerType TargetContainer,
		int32 FromIndex,
		int32 DroppedIndex,
		EArmorType ArmorType
		);

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
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerHotbar(int32 Index);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo, int32 LocalEquippedIndex);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void SpawnEquipableFirstPerson(TSubclassOf<AActor> Class, FName SocketName);
	
	void UseHotbarFunction(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastWeaponEquip(AActor* Target, FName SocketName, const EEquipableState& EquippedState);

	UFUNCTION(NetMulticast, Reliable)
	void DequipThirdPerson();

	UFUNCTION(Client, Reliable, Category="Weapon", BlueprintCallable)
	void DequipFirstPerson();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAttack();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MontageMulticast(UAnimMontage* ThirdPersonMontage);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientMontage(UAnimMontage* FirstPersonMontage);
	
	virtual void ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage) override;

	virtual void FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage) override;
};