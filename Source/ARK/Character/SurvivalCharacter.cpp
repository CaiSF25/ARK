

// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SurvivalPlayerController.h"
#include "ARK/PlayerWindow/PlayerWindow.h"
#include "ARK/HarvestingSystem/DestructableHarvestable.h"
#include "ARK/HarvestingSystem/GroundItemMaster.h"
#include "ARK/Interfaces/ArmorItemInterface.h"
#include "ARK/Interfaces/GroundItemInterface.h"
#include "ARK/Items/ArmorMaster.h"
#include "ARK/Items/Equipables/FirstPersonEquipable.h"
#include "ARK/Structures/ConsumableStructs.h"
#include "ARK/Structures/CraftingStructs.h"
#include "ARK/Structures/ExperienceStruct.h"
#include "ARK/Structures/LargeItem.h"
#include "ARK/Structures/ResourceStructure.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

struct FConsumableInfo;
DEFINE_LOG_CATEGORY_STATIC(LogMyCharacter, Log, All);

ASurvivalCharacter::ASurvivalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 胶囊体
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// 相机
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCameraComponent"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// 第一人称角色
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(6.f, -2.5f, -160.f));

	// 第三人称角色
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = false;
	Mesh3P->CastShadow = false;
	Mesh3P->SetRelativeLocation(FVector(6.f, -2.5f, -160.f));

	// 箭头
	Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
	Arrow1->SetupAttachment(FirstPersonCameraComponent);

	// 库存系统
	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));
	PlayerHotBar = CreateDefaultSubobject<UPlayerHotBar>(TEXT("PlayerHotBar"));

	// 数据表
	bReplicates = true;
	EquipableState = EEquipableState::Default;
}

void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FirstPersonInputContext, 0);
			Subsystem->AddMappingContext(UIInputContext, 1);
		}
	}
	
	if (Mesh3P && Mesh3P->GetAnimInstance())
	{
		Mesh3P->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ASurvivalCharacter::OnThirdPersonMontageEnded);
		Mesh3P->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &ASurvivalCharacter::OnThirdPersonNotifyBegin);

		Mesh3P->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ASurvivalCharacter::OnPickUpMontageEnded);
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ASurvivalCharacter::HandleAnyDamage);
	}
	if (Controller && Controller->IsLocalController() && !IsRunningDedicatedServer())
	{
		GetWorld()->GetTimerManager().SetTimer(
			SetupSceneRenderTimer,
			this,
			&ASurvivalCharacter::SetupSceneRender,
			0.5f,
			false
			);
	}

	OnDecreaseStatsOverTime();
}

void ASurvivalCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnTakeAnyDamage.RemoveDynamic(this, &ASurvivalCharacter::HandleAnyDamage);

	Super::EndPlay(EndPlayReason);
}

ASurvivalPlayerController* ASurvivalCharacter::GetSurvivalController() const
{
	if (AController* PC = GetController(); IsValid(PC) && PC->GetClass()->ImplementsInterface(UPlayerControllerInterface::StaticClass()))
	{
		return Cast<ASurvivalPlayerController>(IPlayerControllerInterface::Execute_SurvivalGamePCRef(PC));
	}
	return nullptr;
}

AArmorMaster* ASurvivalCharacter::GetArmorMaster(AItemMaster* ItemMaster)
{
	if (ItemMaster->GetClass()->ImplementsInterface(UArmorItemInterface::StaticClass()))
	{
		return IArmorItemInterface::Execute_GetArmorRef(ItemMaster);
	}
	return nullptr;
}

void ASurvivalCharacter::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
	if (HasAuthority())
	{
		ApplyDamageToPlayer(Damage, DamageCauser);
	}
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASurvivalCharacter, EquipableState);
	DOREPLIFETIME(ASurvivalCharacter, ThirdPersonEquippedItem);
	DOREPLIFETIME(ASurvivalCharacter, ReplicatedEquipSocketName);
	DOREPLIFETIME(ASurvivalCharacter, ReplicatedEquipState);
	DOREPLIFETIME(ASurvivalCharacter, StaminaState);
	DOREPLIFETIME(ASurvivalCharacter, bIsStarving);
	DOREPLIFETIME(ASurvivalCharacter, bIsDehydrated);
	DOREPLIFETIME(ASurvivalCharacter, bIsSprinting);
	DOREPLIFETIME(ASurvivalCharacter, PlayerStats);
	DOREPLIFETIME(ASurvivalCharacter, bWantsToSprint);
	DOREPLIFETIME(ASurvivalCharacter, bClientHasMoveInput);
	DOREPLIFETIME(ASurvivalCharacter, HelmetSlots);
	DOREPLIFETIME(ASurvivalCharacter, ChestSlots);
	DOREPLIFETIME(ASurvivalCharacter, PantsSlots);
	DOREPLIFETIME(ASurvivalCharacter, BootsSlots);
	// DOREPLIFETIME(ASurvivalCharacter, ThirdPersonEquippedItem);
}


void ASurvivalCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}

	const bool bHasInputNow = MovementVector.SizeSquared() > KINDA_SMALL_NUMBER;

	// 仅本地玩家上报状态变化
	if (IsLocallyControlled())
	{
		if (bHasInputNow != bLocalHasMoveInput)
		{
			bLocalHasMoveInput = bHasInputNow;
			// 向服务器上报（仅在变化时发送，避免每帧 RPC）
			ServerReportMoveInput(bLocalHasMoveInput);
		}
	}
}

void ASurvivalCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(-LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASurvivalCharacter::Attack()
{
	if (HasAuthority())
	{
		if (IsValid(ThirdPersonEquippedItem) && ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			IEquipableItem::Execute_UseItemInterface(ThirdPersonEquippedItem, this);
		}
	}
	else
	{
		ServerAttack();
	}
}

void ASurvivalCharacter::Interact()
{
	if (HasAuthority())
	{
		OnOverlapGroundItems();
	}
	else
	{
		ServerInteract();
	}
}

void ASurvivalCharacter::ServerReportMoveInput_Implementation(bool bHasInput)
{
	bClientHasMoveInput = bHasInput;
	EvaluateSprintState();
}

void ASurvivalCharacter::StopSprinting()
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void ASurvivalCharacter::UpdateMovementSpeed() const
{
	if (GetCharacterMovement())
	{
		if (bIsSprinting && PlayerStats.CurrentStamina > 0.f)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
	}
}

void ASurvivalCharacter::HotbarPressed(int32 Index)
{
	Hotbar(Index);
}

void ASurvivalCharacter::OnThirdPersonMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(ThirdPersonEquippedItem))
	{
		if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			IEquipableItem::Execute_EndAnimation(ThirdPersonEquippedItem);
		}
	}
	else
	{
		bIsHarvesting = false;
	}
}

void ASurvivalCharacter::OnThirdPersonNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (IsValid(ThirdPersonEquippedItem))
	{
		if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			IEquipableItem::Execute_NotifyInterface(ThirdPersonEquippedItem);
		}
	}
}

void ASurvivalCharacter::OnPickUpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsHarvesting = false;
}

void ASurvivalCharacter::OnSlotDrop(EContainerType FromContainer, EContainerType TargetContainer, int32 FromIndex,
                                    int32 DroppedIndex, EArmorType ArmorType)
{
	if (HasAuthority())
	{
		HandleSlotDrop(FromContainer, TargetContainer, FromIndex, DroppedIndex, ArmorType);
	}
	else
	{
		ServerOnSlotDrop(FromContainer, TargetContainer, FromIndex, DroppedIndex, ArmorType);
	}
}

void ASurvivalCharacter::OnHarvestItem(FResourceStructure Resource)
{
	if (HasAuthority())
	{
		HarvestItem(Resource);
	}
	else
	{
		ServerHarvestItem(Resource);
	}
}

void ASurvivalCharacter::OnDequipCurItem(int32 Index)
{
	if (HasAuthority())
	{
		DequipCurItem(Index);
	}
	else
	{
		ServerDequipCurItem(Index);
	}
}

void ASurvivalCharacter::OnSpawnEquipableThirdPerson(const TSubclassOf<AActor> Class, const FItemInfo& ItemInfo,
	const int32 LocalEquippedIndex)
{
	if (HasAuthority())
	{
		SpawnEquipableThirdPerson(Class, ItemInfo, LocalEquippedIndex);
	}
	else
	{
		ServerSpawnEquipableThirdPerson(Class, ItemInfo, LocalEquippedIndex);
	}
}

void ASurvivalCharacter::SpawnEquipableThirdPerson(const TSubclassOf<AActor> Class, FItemInfo ItemInfo,
                                                   const int32 LocalEquippedIndex)
{
	EquippedIndex = LocalEquippedIndex;
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* Spawned = World->SpawnActor(Class, nullptr, nullptr, SpawnParams);
		if (!IsValid(Spawned)) return;

		Spawned->SetOwner(this);
		Spawned->SetReplicates(true);
		Spawned->SetReplicateMovement(true);

		ThirdPersonEquippedItem = Spawned;
		
		if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			const AEquipableMaster* EquipRef = IEquipableItem::Execute_GetEquipableRef(ThirdPersonEquippedItem);
			ReplicatedEquipSocketName = EquipRef->EquipableInfo.SocketName;

			// ReplicatedEquipState = EquipRef->EquipableInfo.AnimationState;
			EquipableState = EquipRef->EquipableInfo.AnimationState;

			const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			Spawned->AttachToComponent(Mesh3P, AttachRules, EquipRef->EquipableInfo.SocketName);
			
			ClientSpawnEquipableFirstPerson(EquipRef->EquipableInfo.FirstPersonEquipClass, EquipRef->EquipableInfo.SocketName);
			ClientEquipItem(EquipRef->EquipableInfo);
		}
	}
}

void ASurvivalCharacter::OverlapGroundItems()
{
	if (!IsValid(ThirdPersonEquippedItem))
	{
		if (!bIsHarvesting)
		{
			bIsHarvesting = true;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			// ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

			const TSubclassOf<AActor> ClassFilter = nullptr;

			const TArray<AActor*> ActorsToIgnore = { this };

			if (TArray<AActor*> OutActors; UKismetSystemLibrary::SphereOverlapActors(
				this,
				GetActorLocation(),
				70.f,
				ObjectTypes,
				ClassFilter,
				ActorsToIgnore, 
				OutActors
				))
			{
				HarvestGroundItem(OutActors[0]);
				OnHarvestMontage();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "No Hit");
				bIsHarvesting = false;
			}

			const UWorld* World = GetWorld();
			/*DrawDebugSphere(
				World,
				GetActorLocation(),
				70.f,
				12,
				FColor::Red,
				false,
				3,
				0,
				2
				);*/
		}
	}
}

bool ASurvivalCharacter::CheckIfCanCraftItem(int32 ID, const EContainerType& Container, const ECraftingType& TableType)
{
	const UDataTable* DataTable = GetRecipeDataTable(TableType);
	
	const FName RowName = FName(*FString::FromInt(ID));
	
	static const FString ContextString(TEXT("CheckIfCanCraftItem"));

	UItemContainer* ItemContainer = GetContainer(Container);

	const auto Row = DataTable->FindRow<FItemRecipe>(
		RowName,
		ContextString,
		true
		);

	if (Row)
	{
		return ItemContainer->ContainsItems(Row->RequiredItems);
	}

	return false;
}

std::tuple<FName, EContainerType, float, int32> ASurvivalCharacter::CraftItem(const int32 ItemID, EContainerType Container, const ECraftingType TableType) const
{
	const UDataTable* DataTable = GetRecipeDataTable(TableType);
	
	UItemContainer* ItemContainer = GetContainer(Container);
		
	const FName RowName = FName(*FString::FromInt(ItemID));
	
	static const FString ContextString(TEXT("CraftItem"));
	const auto Row = DataTable->FindRow<FItemRecipe>(
		RowName,
		ContextString,
		true
		);
	if (Row)
	{
		if (ItemContainer->ContainsItems(Row->RequiredItems))
		{
			ItemContainer->RemoveItems(Row->RequiredItems);
			if (GetController() && GetController()->GetClass()->ImplementsInterface(UPlayerControllerInterface::StaticClass()))
			{
				if (ASurvivalPlayerController* SurvivalController = Cast<ASurvivalPlayerController>(IPlayerControllerInterface::Execute_SurvivalGamePCRef(GetController())))
				{
					SurvivalController->ShowCraftingProgressBar(Row->CraftTime);
				}
			}
			
			return { RowName, Container, Row->CraftTime, Row->ItemExperience };
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CraftItem");
			return { FName(), Container, 0, 0 };
		}
	}
	return { FName(), Container, 0, 0 };
}

void ASurvivalCharacter::ClientSpawnEquipableFirstPerson_Implementation(TSubclassOf<AActor> Class, FName SocketName)
{
	if (!IsLocallyControlled()) return;

	if (IsValid(FirstPersonEquippedItem))
	{
		FirstPersonEquippedItem->Destroy();
		FirstPersonEquippedItem = nullptr;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedFP = GetWorld()->SpawnActor<AActor>(Class, FTransform::Identity, SpawnParams);
	if (!IsValid(SpawnedFP)) return;

	SpawnedFP->SetReplicates(false);

	const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	SpawnedFP->AttachToComponent(Mesh1P, AttachRules, SocketName);

	FirstPersonEquippedItem = SpawnedFP;
}


void ASurvivalCharacter::UseHotbarFunction(int32 Index)
{
	HorbarIndex = Index;
	if (const EItemType ItemType = PlayerHotBar->CheckHotbar(Index); ItemType != EItemType::None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "UseHotbarFunction");
		switch (ItemType)
		{
		case EItemType::Resource:
			break;
		case EItemType::Equipable:
			if (IsValid(ThirdPersonEquippedItem))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Item is already equipped");
				ThirdPersonEquippedItem->Destroy();
				DequipThirdPerson();
				DequipFirstPerson();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Equip the item");
				const FItemInfo EquippedItemInfo = PlayerHotBar->GetItems()[HorbarIndex];
				OnSpawnEquipableThirdPerson(EquippedItemInfo.ItemClassRef, EquippedItemInfo, HorbarIndex);
			}
			break;
		case EItemType::Armor:
			break;
		case EItemType::Consumable:
			ConsumeItem(HorbarIndex, EContainerType::PlayerHotbar);
			break;
		case EItemType::Buildable:
			break;
			
		default:
			break;
		
					
		}
	}
}

void ASurvivalCharacter::Hotbar(const int32 Index)
{
	if (HasAuthority())
	{
		UseHotbarFunction(Index);
	}
	else
	{
		ServerHotbar(Index);
	}
}

void ASurvivalCharacter::ClientMontage_Implementation(UAnimMontage* FirstPersonMontage)
{
	if (!Mesh1P || !FirstPersonMontage) return;
	if (UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance())
	{
		AnimInstance->Montage_Play(FirstPersonMontage);
	}
}

void ASurvivalCharacter::MontageMulticast_Implementation(UAnimMontage* ThirdPersonMontage)
{
	if (!Mesh3P || !ThirdPersonMontage) return;
	if (UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ThirdPersonMontage);
	}
}

void ASurvivalCharacter::OnOverlapGroundItems()
{
	if (HasAuthority())
	{
		OverlapGroundItems();
	}
	else
	{
		ServerOverlapGroundItems();
	}
}

void ASurvivalCharacter::HarvestMontage()
{
	if (UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance())
	{
		AnimInstance->Montage_Play(PickUpMontage);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASurvivalCharacter::OnMontageCompleted);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate);
			
		ClientMontage(PickUpMontage);
		MontageMulticast(PickUpMontage);
		MulticastBush();
	}
}

void ASurvivalCharacter::OnHarvestMontage()
{
	if (HasAuthority())
	{
		HarvestMontage();
	}
	else
	{
		ServerHarvestMontage();
	}
}

void ASurvivalCharacter::OnStartDrainStamina()
{
	if (HasAuthority())
	{
		if (StaminaState != EStaminaState::Draining && PlayerStats.CurrentStamina > 0.f)
		{
			StaminaState = EStaminaState::Draining;
		}
	}
	else
	{
		ServerStartDrainStamina();
	}
}

void ASurvivalCharacter::OnStopDrainStamina()
{
	if (HasAuthority())
	{
		if (StaminaState != EStaminaState::Regenerating)
		{
			StaminaState = EStaminaState::Regenerating;
		}
	}
	else
	{
		ServerStopDrainStamina();
	}
}

void ASurvivalCharacter::OnCheckIfCanCraftItem(const int32 ID, const EContainerType& Container,
                                               const ECraftingType& TableType)
{
	if (HasAuthority())
	{
		const bool CanCraft = CheckIfCanCraftItem(ID, Container, TableType);
		if (GetController() && GetController()->GetClass()->ImplementsInterface(UPlayerControllerInterface::StaticClass()))
		{
			if (ASurvivalPlayerController* SurvivalController = Cast<ASurvivalPlayerController>(IPlayerControllerInterface::Execute_SurvivalGamePCRef(GetController())))
			{
				SurvivalController->UpdateCraftStatus(CanCraft);
			}
		}
	}
	else
	{
		ServerCheckIfCanCraftItem(ID, Container, TableType);
	}
}

void ASurvivalCharacter::OnApplySkillPoints(const EStatEnum& Stat)
{
	if (HasAuthority())
	{
		ApplySkillPoints(Stat);
	}
	else
	{
		ServerApplySkillPoints(Stat);
	}
}

void ASurvivalCharacter::OnCraftItem(const int32 ItemID, const EContainerType Container, const ECraftingType TableType)
{
	if (HasAuthority())
	{
		if (bIsCrafting) return;
		bIsCrafting = true;
		const std::tuple<FName, EContainerType, float, int32> Result = CraftItem(ItemID, Container, TableType);
		const FName ItemIDToAdd = std::get<0>(Result);
		const EContainerType ItemContainer = std::get<1>(Result);
		const float CraftTime = std::get<2>(Result);
		const int32 ExperienceToAdd = std::get<3>(Result);
		if (ItemIDToAdd != FName())
		{
			if (const UWorld* World = GetWorld())
			{
				FTimerDelegate TimerDel;
				TimerDel.BindLambda([this, ItemIDToAdd, ItemContainer, ExperienceToAdd]()
				{
					AddCraftedItem(ItemIDToAdd, ItemContainer);
					OnAddExperience(ExperienceToAdd);
					bIsCrafting = false;
				});
				World->GetTimerManager().SetTimer(
					DelayHandle_CraftItem,
					TimerDel,                                  
					CraftTime,
					false
					);
			}
		}
		else bIsCrafting = false;
	}
	else
	{
		ServerCraftItem(ItemID, Container, TableType);
	}
}

void ASurvivalCharacter::OnRep_EquipableState()
{
}

void ASurvivalCharacter::OnRep_EquippedWeapon()
{
	if (!IsValid(ThirdPersonEquippedItem) && IsValid(Mesh3P))
	{
		const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		ThirdPersonEquippedItem->AttachToComponent(Mesh3P, AttachRules, ReplicatedEquipSocketName);
	}
}

void ASurvivalCharacter::ServerAttack_Implementation()
{
	if (IsValid(ThirdPersonEquippedItem))
	{
		if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			IEquipableItem::Execute_UseItemInterface(ThirdPersonEquippedItem, this);
		}
	}
}

void ASurvivalCharacter::MulticastWeaponEquip_Implementation(AActor* Target, const FName& SocketName,
                                                             const EEquipableState& EquippedState)
{
	if (!IsValid(Target)) return;
	if (!IsValid(Mesh3P)) return;
	const FAttachmentTransformRules AttachmentRules(
			EAttachmentRule::SnapToTarget,    // Location
			EAttachmentRule::SnapToTarget,    // Rotation
			EAttachmentRule::SnapToTarget,    // Scale
			true                              // Weld simulated bodies?
			);
	Target->AttachToComponent(Mesh3P, AttachmentRules, SocketName);
	EquipableState = EquippedState;
}

void ASurvivalCharacter::DequipThirdPerson_Implementation()
{
	EquipableState = EEquipableState::Default;
	ThirdPersonEquippedItem = nullptr;
}

void ASurvivalCharacter::DequipFirstPerson_Implementation()
{
	FirstPersonEquippedItem->Destroy();
	FirstPersonEquippedItem = nullptr;
	if (IsValid(PlayerWindow))
	{
		PlayerWindow->DequipItem();
	}
}

void ASurvivalCharacter::DequipCurItem(const int32 Index)
{
	if (Index == EquippedIndex)
	{
		if (IsValid(ThirdPersonEquippedItem))
		{
			ThirdPersonEquippedItem->Destroy();
			DequipThirdPerson();
			DequipFirstPerson();
		}
	}
}

void ASurvivalCharacter::ServerHotbar_Implementation(const int32 Index)
{
	UseHotbarFunction(Index);
}

void ASurvivalCharacter::ServerEquipArmor_Implementation(const EContainerType& FromContainer, int32 FromIndex,
	const EArmorType& ArmorTypeSlot)
{
	EquipArmor(FromContainer, FromIndex, ArmorTypeSlot);
}

AItemMaster*& ASurvivalCharacter::GetArmorSlotRefByType(const EArmorType Type)
{
	switch (Type)
	{
	case EArmorType::Helmet: return HelmetSlots;
	case EArmorType::Chest: return ChestSlots;
	case EArmorType::Pants: return PantsSlots;
	case EArmorType::Boots: return BootsSlots;
	default: return HelmetSlots;
	}
}

void ASurvivalCharacter::EquipArmor(const EContainerType& FromContainer, const int32 FromIndex,
                                    const EArmorType& ArmorTypeSlot)
{
	if (FromContainer != EContainerType::PlayerInventory) return;
	if (!PlayerInventory) return;
	const TArray<FItemInfo>& Items = PlayerInventory->GetItems();
	if (!Items.IsValidIndex(FromIndex)) return;

	const FItemInfo& LocalItem = Items[FromIndex];
	if (LocalItem.ItemType != EItemType::Armor || LocalItem.ArmorType != ArmorTypeSlot) return;

	ClientEquipArmor(LocalItem);

	AItemMaster*& SlotRef = GetArmorSlotRefByType(ArmorTypeSlot);
	if (IsValid(SlotRef))
	{
		// 已有装备
		return;
	}

	UWorld* World = GetWorld();
	if (!World || !LocalItem.ItemClassRef) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	AItemMaster* NewArmor = World->SpawnActor<AItemMaster>(LocalItem.ItemClassRef, SpawnParameters);
	if (!IsValid(NewArmor)) return;

	NewArmor->SetOwner(this);
	NewArmor->SetReplicates(true);

	SlotRef = NewArmor;

	NewArmor->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);
	if (AArmorMaster* ArmorMaster = GetArmorMaster(NewArmor))
	{
		ArmorMaster->MasterPoseEvent(this);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ASurvivalPlayerController* SurvivalPC = Cast<ASurvivalPlayerController>(PC))
		{
			if (AArmorMaster* AM = GetArmorMaster(NewArmor))
			{
				SurvivalPC->UpdateArmorUI(ArmorTypeSlot, AM->GetItemInfo());
				PlayerInventory->RemoveItemAtIndex(FromIndex);
			}
		}
	}
}

void ASurvivalCharacter::ServerDequipArmor_Implementation(const EArmorType& ArmorSlot)
{
	DequipArmor(ArmorSlot);
}

void ASurvivalCharacter::DequipArmor(const EArmorType& ArmorSlot)
{
	AItemMaster*& SlotRef = GetArmorSlotRefByType(ArmorSlot);
	if (!IsValid(SlotRef)) return;
	if (!SlotRef->GetClass()->ImplementsInterface(UArmorItemInterface::StaticClass()))
		return;
	AArmorMaster* ArmorRef = IArmorItemInterface::Execute_GetArmorRef(SlotRef);
	if (!ArmorRef) return;
	PlayerInventory->ServerAddItem(ArmorRef->GetItemInfo());
	SlotRef->Destroy();
	SlotRef = nullptr;
	GetSurvivalController()->RemoveArmorUI(ArmorSlot);
	ClientDequipArmor(ArmorSlot);
}

void ASurvivalCharacter::PostComp(AItemMaster* Target)
{
	if (IsValid(Target))
	{
		Target->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale,"None");
		if (AArmorMaster* ArmorMaster = GetArmorMaster(Target))
		{
			ArmorMaster->MasterPoseEvent(this);
		}
	}
}

void ASurvivalCharacter::SetupSceneRender_Implementation()
{
	if (bSetupSceneRenderDoOnce) return;
	bSetupSceneRenderDoOnce = true;
	UWorld* World = GetWorld();
	if (!World) return;
	if (!PlayerWindowClass) return;

	FActorSpawnParameters SpawnParams;
	const FVector SpawnLocation = FVector(250.0f, 1090.0f, 9600.0f);
	const FRotator SpawnRotation = FRotator(0.f, 0.f, 90.f);
	const FVector SpawnScale = FVector(1.0f, 1.0f, 1.0f);
	const FTransform SpawnTransform(SpawnRotation, SpawnLocation, SpawnScale);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	PlayerWindow = World->SpawnActor<APlayerWindow>(PlayerWindowClass, SpawnTransform, SpawnParams);

	if (!PlayerWindow) return;
	ASurvivalPlayerController* SurvivalPC = GetSurvivalController();
	if (!SurvivalPC) return;
	PlayerWindow->InitialWindow(SurvivalPC);
}

void ASurvivalCharacter::ClientDequipArmor_Implementation(const EArmorType& ArmorType)
{
	GetPlayerWindow()->DequipArmor(ArmorType);
}

void ASurvivalCharacter::ConsumeItem(const int32 Index, const EContainerType& Container)
{
	if (UItemContainer* ItemContainer = GetContainer(Container))
	{
		const int32 LocalItemID = ItemContainer->GetItems()[Index].ItemID;
		if (ItemContainer->RemoveQuantity(Index, 1).first)
		{
			const FName RowName = FName(FString::FromInt(LocalItemID));
			static const FString ContextString(TEXT("CraftItem"));
			if (!ConsumeableDataTable) return;
			const auto Row = ConsumeableDataTable->FindRow<FConsumableInfo>(
				RowName,
				ContextString,
				true
				);
			if (Row)
			{
				for (const auto& Stats : Row->StatsToUpdate)
				{
					if (Stats.bInstantEffect)
					{
						UpdateStatInstant(Stats.StatToModify, Stats.RestoreAmount);
					}
					if (Stats.bOverTime)
					{
						UpdateStatOverTime(Stats.StatToModify, Stats.OverTimeAmount, Stats.OverTimeDuration);
					}
				}
			}
		}
	}
}

void ASurvivalCharacter::UpdateStatInstant(EStatEnum StatToChange, float Amount)
{
	const float LocalStat = FMath::Clamp(Amount + GetStateToUpdate(StatToChange), 0.f, GetMaxState(StatToChange));

	switch (StatToChange)
	{
	case EStatEnum::Health:
		PlayerStats.CurrentHealth = LocalStat;
		break;
	case EStatEnum::Food:
		if (bIsStarving)
		{
			bIsStarving = false;
		}
		PlayerStats.CurrentFood = LocalStat;
		break;
	case EStatEnum::Water:
		if (bIsDehydrated)
		{
			bIsDehydrated = false;
		}
		PlayerStats.CurrentWater = LocalStat;
		break;
	case EStatEnum::Stamina:
		PlayerStats.CurrentStamina = LocalStat;
		break;
	default:
		break;
	}

	ASurvivalPlayerController* SurvivalController = GetSurvivalController();
	if (SurvivalController)
	{
		switch (StatToChange)
		{
		case EStatEnum::Health:
			UpdateHealthUI();
			break;
		case EStatEnum::Food:
			SurvivalController->ShowOrHideStarving(false);
			UpdateFoodUI();
			break;
		case EStatEnum::Water:
			SurvivalController->ShowOrHideDehydrated(false);
			UpdateWaterUI();
			break;
		case EStatEnum::Stamina:
			UpdateStaminaUI();
			break;
		default:
			break;
		}
	}
}

void ASurvivalCharacter::HealthOverTime()
{
	if (HealthTicksRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthHandle);
		return;
	}
	UpdateStatInstant(EStatEnum::Health, HealthAmountPerTick);
	
	HealthTicksRemaining -= 1;
	if (HealthTicksRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthHandle);
	}
}

void ASurvivalCharacter::FoodOverTime()
{
	if (FoodTicksRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FoodHandle);
		return;
	}
	UpdateStatInstant(EStatEnum::Food, FoodAmountPerTick);
	
	FoodTicksRemaining -= 1;
	if (FoodTicksRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FoodHandle);
	}
}

void ASurvivalCharacter::WaterOverTime()
{
	if (WaterTicksRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaterHandle);
		return;
	}
	UpdateStatInstant(EStatEnum::Water, WaterAmountPerTick);
	
	WaterTicksRemaining -= 1;
	if (WaterTicksRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaterHandle);
	}
}

void ASurvivalCharacter::UpdateStatOverTime(EStatEnum StatToChange, float OverTimeAmount, float Duration)
{
	constexpr  float TickInterval = 1.0f;
	const int32 Ticks = FMath::Max(1, FMath::CeilToInt(Duration / TickInterval));
	const float Interval = Duration / static_cast<float>(Ticks);
	switch (StatToChange)
	{
	case EStatEnum::Health:
		{
			HealthTicksRemaining = Ticks;
			HealthAmountPerTick = OverTimeAmount / static_cast<float>(Ticks);
			
			GetWorld()->GetTimerManager().SetTimer(
				HealthHandle,
				this,
				&ASurvivalCharacter::HealthOverTime,                                  
				Interval,
				true
				);
		}
		break;
	case EStatEnum::Food:
		{
			FoodTicksRemaining = Ticks;
			FoodAmountPerTick = OverTimeAmount / static_cast<float>(Ticks);
			
			GetWorld()->GetTimerManager().SetTimer(
				FoodHandle,
				this,
				&ASurvivalCharacter::FoodOverTime,                                  
				Interval,
				true
				);
		}
		break;
	case EStatEnum::Water:
		{
			WaterTicksRemaining = Ticks;
			WaterAmountPerTick = OverTimeAmount / static_cast<float>(Ticks);
			
			GetWorld()->GetTimerManager().SetTimer(
				WaterHandle,
				this,
				&ASurvivalCharacter::WaterOverTime,                                  
				Interval,
				true
				);
		}
		break;
	case EStatEnum::Stamina:
		break;
	default:
		break;
	}
}

void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSprinting && PlayerStats.CurrentStamina <= 0.f)
	{
		StopSprinting();
	}

	ProcessStamina(DeltaTime);

	UpdateMovementSpeed();
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 角色核心动作
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASurvivalCharacter::Interact);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASurvivalCharacter::Attack);
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ASurvivalCharacter::OnSprintPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASurvivalCharacter::OnSprintReleased);
		
		// UI 操作
		for (int32 i = 0; i < HotbarActions.Num(); i++)
		{
			EnhancedInputComponent->BindAction(
				HotbarActions[i],
				ETriggerEvent::Started,
				this,
				&ASurvivalCharacter::HotbarPressed,
				i);
		}
	}
}

void ASurvivalCharacter::ServerHarvestItem_Implementation(FResourceStructure Resource)
{
	HarvestItem(Resource);
}

void ASurvivalCharacter::HarvestItem(FResourceStructure Resource)
{
	const FName RowName = Resource.ResourceID;
				
	static const FString ContextString(TEXT("HandleTreeHit"));
	FItemInfo* Row = ItemsDataTable->FindRow<FItemInfo>(
		RowName,
		ContextString,
		true
		);
	if (Row)
	{
		Row->ItemQuantity = Resource.Quantity;
		PlayerInventory->ServerAddItem(*Row);

		if (GetController() && GetController()->GetClass()->ImplementsInterface(UPlayerControllerInterface::StaticClass()))
		{
			ASurvivalPlayerController* SurvivalController = Cast<ASurvivalPlayerController>(IPlayerControllerInterface::Execute_SurvivalGamePCRef(GetController()));
			if (SurvivalController)
			{
				SurvivalController->ShowItemWidget(Row->ItemIcon, Row->ItemQuantity, Row->ItemName);
			}
		}
	}
}

void ASurvivalCharacter::ClientEquipItem_Implementation(const FEquipableInfo& Info)
{
	GetPlayerWindow()->EquipItem(Info);
}

void ASurvivalCharacter::OnRep_HelmetSlots()
{
	PostComp(HelmetSlots);
}

void ASurvivalCharacter::OnRep_ChestSlots()
{
	PostComp(ChestSlots);
}

void ASurvivalCharacter::OnRep_PantsSlots()
{
	PostComp(PantsSlots);
}

void ASurvivalCharacter::OnRep_BootsSlots()
{
	PostComp(BootsSlots);
}

void ASurvivalCharacter::OnEquipArmor(const EContainerType& FromContainer, const int32 FromIndex,
                                      const EArmorType& ArmorTypeSlot)
{
	if (HasAuthority())
	{
		EquipArmor(FromContainer, FromIndex, ArmorTypeSlot);
	}
	else
	{
		ServerEquipArmor(FromContainer, FromIndex, ArmorTypeSlot);
	}
}

void ASurvivalCharacter::OnDequipArmor(const EArmorType& ArmorSlot)
{
	if (HasAuthority())
	{
		DequipArmor(ArmorSlot);
	}
	else
	{
		ServerDequipArmor(ArmorSlot);
	}
}

void ASurvivalCharacter::ClientEquipArmor_Implementation(const FItemInfo& ItemInfo)
{
	GetPlayerWindow()->EquipArmor(ItemInfo);
}


void ASurvivalCharacter::HarvestGroundItem(AActor* Ref)
{
	if (Ref->GetClass()->ImplementsInterface(UGroundItemInterface::StaticClass()))
	{
		if (AGroundItemMaster* GroundItem = IGroundItemInterface::Execute_GetGroundItemRef(Ref))
		{
			const FName RowName = FName(*GroundItem->GetClass()->GetName());
			static const FString ContextString(TEXT("HandleInteract"));

			FLargeItem* Row = GroundResourcesTable->FindRow<FLargeItem>(
					RowName,
					ContextString,
					true);

			if (const float LocalHealth = GroundItem->GetHealth() - 15; LocalHealth <= 0)
			{
				if (Row)
				{
					if (IsValid(Row->Class))
					{
						const FTransform Transform = GroundItem->GetTransform();
						GroundItem->Destroy();
						if (UWorld* World = GroundItem->GetWorld())
						{
							World->SpawnActor(Row->Class, &Transform);
						}
					}
				}
			}
			else
			{
				GroundItem->SetHealth(LocalHealth);
				if (Row)
				{
					for (FResourceStructure Resource : Row->GivenItems)
					{
						FResourceStructure LocalResource = Resource;
						const float BaseVar = Resource.Quantity;
						const float RateVar = 1;
						const float RandomVar = FMath::RandRange(0.3, 1.2);
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f, %f, %f"), BaseVar, RateVar, RandomVar));

						if (int32 LocalQuantity = FMath::RoundToFloat(BaseVar * RateVar * RandomVar) > 0)
						{
							LocalResource.Quantity = LocalQuantity;
							OnHarvestItem(LocalResource);
						}
					}
				}
			}
		}
	}
}

void ASurvivalCharacter::MulticastBush_Implementation()
{
	if (const UWorld* World = GetWorld())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("MulticastBush"));
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			BushHarvestParticle,
			GetActorLocation()
	   );

		UGameplayStatics::PlaySoundAtLocation(
			World,
			BushHarvestSound,
			GetActorLocation(),
			1,
			1,
			0,
			BushHarvestAttenuation
			);
	}
}

void ASurvivalCharacter::OnDelayFinished(const FName& ItemIDToAdd, const EContainerType& Container) const
{
	AddCraftedItem(ItemIDToAdd, Container);
}

void ASurvivalCharacter::AddCraftedItem(const FName& ItemIDToAdd, const EContainerType& Container) const
{
	static const FString ContextString(TEXT("AddCraftedItem"));
	auto Row = ItemsDataTable->FindRow<FItemInfo>(
		ItemIDToAdd,
		ContextString,
		true
		);
	auto OutRow = Row;
	OutRow->ItemQuantity = 1;

	UItemContainer* ItemContainer = GetContainer(Container);

	ItemContainer->ServerAddItem(*Row);
}

UDataTable* ASurvivalCharacter::GetRecipeDataTable(const ECraftingType& TableType) const
{
	UDataTable* RecipeDataTable = nullptr;
	switch (TableType)
	{
	case ECraftingType::PlayerInventory:
		if (PlayerItemRecipeDataTable)
		{
			RecipeDataTable = PlayerItemRecipeDataTable;
		}
		break;
	case ECraftingType::CookingPot:
		break;
	case ECraftingType::CraftingBench:
		break;
	case ECraftingType::Forge:
		break;
	case ECraftingType::StorageBox:
		break;
	}
	return RecipeDataTable;
}

UItemContainer* ASurvivalCharacter::GetContainer(const EContainerType& ContainerType) const
{
	UItemContainer* Container = nullptr;
	
	switch (ContainerType)
	{
	case EContainerType::PlayerInventory:
		Container = PlayerInventory;
		break;
	case EContainerType::PlayerHotbar:
		Container = PlayerHotBar;
		break;
	case EContainerType::PlayerArmor:
		break;
	case EContainerType::PlayerStorage:
		break;
	}
	return Container;
}

float ASurvivalCharacter::GetStateToUpdate(const EStatEnum& State)
{
	float Result;
	switch (State)
	{
	case EStatEnum::Health:
		Result = PlayerStats.CurrentHealth;
		break;
	case EStatEnum::Food:
		Result = PlayerStats.CurrentFood;
		break;
	case EStatEnum::Water:
		Result = PlayerStats.CurrentWater;
		break;
	case EStatEnum::Stamina:
		Result = PlayerStats.CurrentStamina;
		break;
	default:
		Result = PlayerStats.CurrentHealth;
		break;
	}
	return Result;
}

float ASurvivalCharacter::GetMaxState(const EStatEnum& State)
{
	float Result;
	switch (State)
	{
	case EStatEnum::Health:
		Result = PlayerStats.MaxHealth;
		break;
	case EStatEnum::Food:
		Result = PlayerStats.MaxFood;
		break;
	case EStatEnum::Water:
		Result = PlayerStats.MaxWater;
		break;
	case EStatEnum::Stamina:
		Result = PlayerStats.MaxStamina;
		break;
	default:
		Result = PlayerStats.MaxHealth;
		break;
	}
	return Result;
}

APlayerWindow* ASurvivalCharacter::GetPlayerWindow() const
{
	if (PlayerWindow && PlayerWindow->GetClass()->ImplementsInterface(UPlayerWindowInterface::StaticClass()))
	{
		return IPlayerWindowInterface::Execute_GetPlayerWindowRef(PlayerWindow);
	}
	return nullptr;
}

void ASurvivalCharacter::ApplyDamageToPlayer(float Damage, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Damage!!!!!!!");
	if (!bIsDead)
	{
		ArmorDuraDamageMele(Damage);
		const float LocalCurHealth = PlayerStats.CurrentHealth - Damage;
		PlayerStats.CurrentHealth = LocalCurHealth;
		if (GetController() && GetController()->GetClass()->ImplementsInterface(UPlayerControllerInterface::StaticClass()))
		{
			ASurvivalPlayerController* SurvivalController = Cast<ASurvivalPlayerController>(IPlayerControllerInterface::Execute_SurvivalGamePCRef(GetController()));
			if (SurvivalController)
			{
				if (LocalCurHealth <= 0)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player is Dead")));
					bIsDead = true;
					SurvivalController->UpdateStatBar(EStatEnum::Health, 0, PlayerStats.MaxHealth);
				}
				else
				{
					SurvivalController->UpdateStatBar(EStatEnum::Health, LocalCurHealth, PlayerStats.MaxHealth);
				}
			}
		}
	}
	
}

void ASurvivalCharacter::ArmorDuraDamageMele(float Damage)
{
	const float LocalDamage = Damage / StaticCast<float>(Execute_GetTotalArmorPieces(this)) / 2.f;
	if (IsValid(HelmetSlots))
	{
		ArmorSlotDamage(HelmetSlots, EArmorType::Helmet, LocalDamage);
	}

	if (IsValid(ChestSlots))
	{
		ArmorSlotDamage(ChestSlots, EArmorType::Chest, LocalDamage);
	}

	if (IsValid(PantsSlots))
	{
		ArmorSlotDamage(PantsSlots, EArmorType::Pants, LocalDamage);
	}

	if (IsValid(BootsSlots))
	{
		ArmorSlotDamage(BootsSlots, EArmorType::Boots, LocalDamage);
	}
}

void ASurvivalCharacter::ArmorSlotDamage(UObject* ArmorRef, const EArmorType& ArmorType, const float DuraDamage)
{
	if (!ArmorRef->GetClass()->ImplementsInterface(UArmorItemInterface::StaticClass())) return;
	AArmorMaster* ArmorMaster = IArmorItemInterface::Execute_GetArmorRef(ArmorRef);
	if (!ArmorMaster) return;
	
	FItemInfo NewItemInfo = ArmorMaster->GetItemInfo();
	const int32 Durability = NewItemInfo.ItemCurHp;

	if (const int32 NewDurability = Durability - FMath::TruncToInt32(DuraDamage); NewDurability <= 0)
	{
		ArmorBreak(ArmorType);
	}
	else
	{
		NewItemInfo.ItemCurHp = NewDurability;
		ArmorMaster->SetItemInfo(NewItemInfo);
		GetSurvivalController()->UpdateArmorUI(ArmorType, NewItemInfo);
	}
}

void ASurvivalCharacter::ArmorBreak(const EArmorType& ArmorType)
{
	AItemMaster*& ArmorSlot = GetArmorSlotRefByType(ArmorType);
	if (!IsValid(ArmorSlot)) return;

	ArmorSlot->Destroy();
	ArmorSlot = nullptr;
	GetSurvivalController()->RemoveArmorUI(ArmorType);
	ClientDequipArmor(ArmorType);
}

void ASurvivalCharacter::OnRep_Starving()
{
	if (ASurvivalPlayerController* SurvivalController = GetSurvivalController())
	{
		SurvivalController->ShowOrHideStarving(bIsStarving);
	}
}

void ASurvivalCharacter::OnRep_Dehydrated()
{
	if (ASurvivalPlayerController* SurvivalController = GetSurvivalController())
	{
		SurvivalController->ShowOrHideDehydrated(bIsDehydrated);
	}
}

void ASurvivalCharacter::OnRep_PlayerStats()
{
	if (PlayerStats.CurrentStamina <= 0.f && bIsSprinting)
	{
		bIsSprinting = false;
		UpdateMovementSpeed();
	}
	UpdateStaminaUI();
}

float ASurvivalCharacter::DecreaseFloat(const float FloatToDecrease, const float Percentage, const float Max)
{
	return FMath::Clamp(FloatToDecrease - Percentage * Max, 0.f, Max);
}

void ASurvivalCharacter::FoodWaterDrain()
{
	const float FoodFloat = DecreaseFloat(PlayerStats.CurrentFood, 0.02f, PlayerStats.MaxFood);
	const float WaterFloat = DecreaseFloat(PlayerStats.CurrentWater, 0.04f, PlayerStats.MaxWater);
    
	ASurvivalPlayerController* SurvivalController = GetSurvivalController();
	bool bWasStarving = bIsStarving;
	bool bWasDehydrated = bIsDehydrated;
	
	// 饥饿状态处理
	bIsStarving = FoodFloat <= 0;
	if (bIsStarving != bWasStarving)
	{
		if (SurvivalController)
		{
			SurvivalController->ShowOrHideStarving(bIsStarving);
		}
	}
	
	// 脱水状态处理
	bIsDehydrated = WaterFloat <= 0;
	if (bIsDehydrated != bWasDehydrated)
	{
		if (SurvivalController)
		{
			SurvivalController->ShowOrHideDehydrated(bIsDehydrated);
		}
	}

	// 更新玩家状态
	PlayerStats.CurrentFood = FoodFloat;
	PlayerStats.CurrentWater = WaterFloat;

	// 健康检查
	if (bIsStarving || bIsDehydrated)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(DecreaseHealthHandle))
		{
			OnDecreaseHealthOverTime();
		}
	}
	else if (GetWorld()->GetTimerManager().IsTimerActive(DecreaseHealthHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DecreaseHealthHandle);
	}

	// 更新UI
	if (SurvivalController)
	{
		SurvivalController->UpdateStatBar(EStatEnum::Food, PlayerStats.CurrentFood, PlayerStats.MaxFood);
		SurvivalController->UpdateStatBar(EStatEnum::Water, PlayerStats.CurrentWater, PlayerStats.MaxWater);
	}
}

void ASurvivalCharacter::PassiveStatDrain()
{
	if (bIsDead)
	{
		GetWorld()->GetTimerManager().ClearTimer(DecreaseStatsHandle);
		GetWorld()->GetTimerManager().ClearTimer(DecreaseHealthHandle);
	}
	else
	{
		FoodWaterDrain();
	}
}

void ASurvivalCharacter::DecreaseStatsOverTime()
{
	GetWorld()->GetTimerManager().ClearTimer(DecreaseStatsHandle);
	GetWorld()->GetTimerManager().SetTimer(
		DecreaseStatsHandle,                     // 定时器句柄
		this,                                  // 拥有定时器的对象
		&ASurvivalCharacter::PassiveStatDrain, // 要调用的函数
		10.f,                                  // 调用间隔（秒）
		true);
}

void ASurvivalCharacter::ServerDecreaseStatsOverTime_Implementation()
{
	DecreaseStatsOverTime();
}

void ASurvivalCharacter::OnDecreaseStatsOverTime()
{
	if (HasAuthority())
	{
		DecreaseStatsOverTime();
	}
	else
	{
		ServerDecreaseStatsOverTime();
	}
}

void ASurvivalCharacter::UpdateFoodUI() const
{
	if (ASurvivalPlayerController* SurvivalController = GetSurvivalController())
	{
		SurvivalController->UpdateStatBar(
			EStatEnum::Food, 
			PlayerStats.CurrentFood,
			PlayerStats.MaxFood
		);
	}
}

void ASurvivalCharacter::UpdateWaterUI() const
{
	if (ASurvivalPlayerController* SurvivalController = GetSurvivalController())
	{
		SurvivalController->UpdateStatBar(
			EStatEnum::Water, 
			PlayerStats.CurrentWater,
			PlayerStats.MaxWater
		);
	}
}

void ASurvivalCharacter::CheckStartHealthDrain()
{
	if ((bIsStarving || bIsDehydrated) && !GetWorld()->GetTimerManager().IsTimerActive(DecreaseHealthHandle))
	{
		OnDecreaseHealthOverTime();
	}
}

void ASurvivalCharacter::CheckStopHealthDrain()
{
	if (!bIsStarving && !bIsDehydrated && GetWorld()->GetTimerManager().IsTimerActive(DecreaseHealthHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DecreaseHealthHandle);
	}
}

void ASurvivalCharacter::DecreaseHealth()
{
	if (bIsDead)
	{
		GetWorld()->GetTimerManager().ClearTimer(DecreaseHealthHandle);
	}
	else
	{
		if (bIsStarving && bIsDehydrated)
		{
			RemoveHealth(4);
		}
		else
		{
			RemoveHealth(2);
		}
	}
}

void ASurvivalCharacter::RemoveHealth(float Amount)
{
	UGameplayStatics::ApplyDamage(this, Amount, GetController(), this, UDamageType::StaticClass());
}

void ASurvivalCharacter::UpdateHealthUI() const
{
	if (ASurvivalPlayerController* SurvivalController = GetSurvivalController())
	{
		SurvivalController->UpdateStatBar(
			EStatEnum::Health, 
			PlayerStats.CurrentHealth,
			PlayerStats.MaxHealth
		);
	}
}

void ASurvivalCharacter::DecreaseHealthOverTime()
{
	GetWorld()->GetTimerManager().ClearTimer(DecreaseHealthHandle);
	GetWorld()->GetTimerManager().SetTimer(
		DecreaseHealthHandle,               // 定时器句柄
		this,                                  // 拥有定时器的对象
		&ASurvivalCharacter::DecreaseHealth,   // 要调用的函数
		15.f,                                  // 调用间隔（秒）
		true,
		1);
}

void ASurvivalCharacter::OnDecreaseHealthOverTime()
{
	if(!(bIsStarving || bIsDehydrated) || bIsDead) return;
	if (HasAuthority())
	{
		DecreaseHealthOverTime();
	}
	else
	{
		ServerDecreaseHealthOverTime();
	}
}

void ASurvivalCharacter::ServerDecreaseHealthOverTime_Implementation()
{
	DecreaseHealthOverTime();
}

void ASurvivalCharacter::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	bIsHarvesting = false;
}

void ASurvivalCharacter::HandleSlotDrop(const EContainerType FromContainer, const EContainerType TargetContainer, const int32 FromIndex, const int32 DroppedIndex, EArmorType ArmorType)
{
	if (FromContainer == EContainerType::PlayerArmor)
	{
		OnDequipArmor(ArmorType);
	}
	else
	{
		UItemContainer* From = GetContainer(FromContainer);
		UItemContainer* To = GetContainer(TargetContainer);

		To->ServerOnSlotDrop(From, FromIndex, DroppedIndex);
	}
}

void ASurvivalCharacter::ServerOnSlotDrop_Implementation(EContainerType FromContainer, EContainerType TargetContainer,
	int32 FromIndex, int32 DroppedIndex, EArmorType ArmorType)
{
	HandleSlotDrop(FromContainer, TargetContainer, FromIndex, DroppedIndex, ArmorType);
}


void ASurvivalCharacter::ServerDequipCurItem_Implementation(int32 Index)
{
	DequipCurItem(Index);
}

void ASurvivalCharacter::ServerSpawnEquipableThirdPerson_Implementation(TSubclassOf<AActor> Class, FItemInfo ItemInfo,
	int32 LocalEquippedIndex)
{
	if (!HasAuthority()) return;
	SpawnEquipableThirdPerson(Class, ItemInfo, LocalEquippedIndex);
}

void ASurvivalCharacter::ServerInteract_Implementation()
{
	OnOverlapGroundItems();
}

void ASurvivalCharacter::ServerOverlapGroundItems_Implementation()
{
	OverlapGroundItems();
}

void ASurvivalCharacter::ServerHarvestMontage_Implementation()
{
	HarvestMontage();
}

void ASurvivalCharacter::ServerCheckIfCanCraftItem_Implementation(int32 ID, const EContainerType& Container,
	const ECraftingType& TableType)
{
	const bool CanCraft = CheckIfCanCraftItem(ID, Container, TableType);
	if (GetController() && GetController()->GetClass()->ImplementsInterface(UPlayerControllerInterface::StaticClass()))
	{
		ASurvivalPlayerController* SurvivalController = Cast<ASurvivalPlayerController>(IPlayerControllerInterface::Execute_SurvivalGamePCRef(GetController()));
		if (SurvivalController)
		{
			SurvivalController->UpdateCraftStatus(CanCraft);
		}
	}
}

void ASurvivalCharacter::ServerCraftItem_Implementation(const int32 ItemID, const EContainerType Container, const ECraftingType TableType)
{
	if (bIsCrafting) return;
	bIsCrafting = true;
	const std::tuple<FName, EContainerType, float, int32> Result = CraftItem(ItemID, Container, TableType);
	const FName ItemIDToAdd = std::get<0>(Result);
	const EContainerType ItemContainer = std::get<1>(Result);
	const float CraftTime = std::get<2>(Result);
	const int32 ExperienceToAdd = std::get<3>(Result);
	if (ItemIDToAdd != FName())
	{
		if (const UWorld* World = GetWorld())
		{
			FTimerDelegate TimerDel;
			TimerDel.BindLambda([this, ItemIDToAdd, ItemContainer, ExperienceToAdd]()
			{
				AddCraftedItem(ItemIDToAdd, ItemContainer);
				OnAddExperience(ExperienceToAdd);
				bIsCrafting = false;
			});
			World->GetTimerManager().SetTimer(
				DelayHandle_CraftItem,
				TimerDel,                                  
				CraftTime,
				false
				);
		}
	}
	else bIsCrafting = false;
}

void ASurvivalCharacter::ServerStartDrainStamina_Implementation()
{
	if (StaminaState != EStaminaState::Draining && PlayerStats.CurrentStamina > 0.f)
	{
		StaminaState = EStaminaState::Draining;
	}
}

void ASurvivalCharacter::ServerStopDrainStamina_Implementation()
{
	if (StaminaState != EStaminaState::Regenerating)
	{
		StaminaState = EStaminaState::Regenerating;
	}
}

void ASurvivalCharacter::ServerSetWantsToSprint_Implementation(bool bNewWants)
{
	bWantsToSprint = bNewWants;
}


void ASurvivalCharacter::OnRep_IsSprinting()
{
	UpdateMovementSpeed();
}

void ASurvivalCharacter::OnSprintPressed()
{
	if (IsLocallyControlled())
	{
		ServerSetWantsToSprint(true);
	}
}

void ASurvivalCharacter::OnSprintReleased()
{
	if (IsLocallyControlled())
	{
		ServerSetWantsToSprint(false);
	}
}

void ASurvivalCharacter::EvaluateSprintState()
{
	if (!HasAuthority()) return; // 仅服务器评估

	// 必要条件：玩家想冲刺、有耐力、在地面并且客户端报告存在移动输入
	const bool bHasStamina = PlayerStats.CurrentStamina > 0.f;
	const bool bIsOnGround = GetCharacterMovement() && GetCharacterMovement()->IsMovingOnGround();

	// 使用客户端上报的输入标记（比 velocity 更能区分被击飞）
	const bool bHasMoveInput = bClientHasMoveInput;

	const bool bShouldSprint = bWantsToSprint && bHasStamina && bIsOnGround && bHasMoveInput;

	if (bShouldSprint && !bIsSprinting)
	{
		bIsSprinting = true;
		UpdateMovementSpeed();
	}
	else if (!bShouldSprint && bIsSprinting)
	{
		bIsSprinting = false;
		UpdateMovementSpeed();
	}
}

void ASurvivalCharacter::UpdateStaminaUI()
{
	if (ASurvivalPlayerController* SurvivalController = GetSurvivalController())
	{
		SurvivalController->UpdateStatBar(
			EStatEnum::Stamina, 
			PlayerStats.CurrentStamina,
			PlayerStats.MaxStamina
		);
	}
}

void ASurvivalCharacter::ClientUpdateStaminaUI_Implementation()
{
	UpdateStaminaUI();
}

void ASurvivalCharacter::ProcessStamina(float DeltaTime)
{
	if (!HasAuthority()) return;
	
	EvaluateSprintState();
	const bool bNeedDrain = bIsSprinting;
	
	switch (StaminaState)
    {
        case EStaminaState::Draining:
            if (bNeedDrain)
            {
                // 消耗耐力（保持原逻辑）
                const float NewStamina = FMath::Clamp(
                    PlayerStats.CurrentStamina - StaminaDrainRate * DeltaTime,
                    0.f,
                    PlayerStats.MaxStamina
                );

                if (NewStamina != PlayerStats.CurrentStamina)
                {
                    PlayerStats.CurrentStamina = NewStamina;
                    if (IsLocallyControlled())
                    {
                        UpdateStaminaUI();
                    }
                }

                if (PlayerStats.CurrentStamina <= 0.f)
                {
                    // 耐力耗尽：停止冲刺并转为 Idle
                    bWantsToSprint = false; // 客户端应收到复制后知道不能继续想冲刺
                    bIsSprinting = false;
                    UpdateMovementSpeed();
                    StaminaState = EStaminaState::Idle;
                }
            }
            else
            {
                StaminaState = EStaminaState::Regenerating;
            }
            break;

        case EStaminaState::Regenerating:
            // 保持原有再生逻辑
            if (PlayerStats.CurrentStamina < PlayerStats.MaxStamina)
            {
                const float NewStamina = FMath::Clamp(
                    PlayerStats.CurrentStamina + StaminaRegenRate * DeltaTime,
                    0.f,
                    PlayerStats.MaxStamina
                );

                if (NewStamina != PlayerStats.CurrentStamina)
                {
                    PlayerStats.CurrentStamina = NewStamina;
                    if (IsLocallyControlled()) UpdateStaminaUI();
                }
            }
            else
            {
                StaminaState = EStaminaState::Idle;
            }

            // 如果玩家仍然想冲刺并且恢复了一些耐力，重新评估是否开始耗耐力
            if (bWantsToSprint && PlayerStats.CurrentStamina > 0.f)
            {
                EvaluateSprintState();
                if (bIsSprinting)
                {
                    StaminaState = EStaminaState::Draining;
                }
            }
            break;

        case EStaminaState::Idle:
            if (bIsSprinting)
            {
                StaminaState = EStaminaState::Draining;
            }
            else if (PlayerStats.CurrentStamina < PlayerStats.MaxStamina)
            {
                StaminaState = EStaminaState::Regenerating;
            }
            break;
        default:
            break;
    }
}

int32 ASurvivalCharacter::GetExperienceLevel(int32 Level) const
{
	if (!ExperienceDataTable || Level <= 0) return 0;
	
	Level = FMath::Clamp(Level, 1, 100);
	
	const FName RowName = FName(*FString::FromInt(PlayerStats.CurrentLevel));

	if (const auto ExpData = ExperienceDataTable->FindRow<FExperienceStruct>(RowName, TEXT("Experience")))
	{
		return ExpData->ExperienceNeeded;
	}
	
	return 100 * FMath::Pow(1.1f, Level - 1);
}

void ASurvivalCharacter::AddExperience(const int32 Experience)
{
	if (Experience < 0) return;

	PlayerStats.CurrentXP += Experience;

	int32 CurrentLevel = PlayerStats.CurrentLevel;

	bool bLevelUp = false;
	while (PlayerStats.CurrentLevel < 20 &&
		PlayerStats.CurrentXP >= GetExperienceLevel(PlayerStats.CurrentLevel))
	{
		const int32 RequiredExp = GetExperienceLevel(PlayerStats.CurrentLevel);
		PlayerStats.CurrentLevel++;
		bLevelUp = true;

		PlayerStats.CurrentXP -= RequiredExp;
	}

	const int32 SkillPoints = PlayerStats.CurrentLevel - CurrentLevel;
	PlayerStats.SkillPoints += SkillPoints;

	if (PlayerStats.CurrentLevel >= 20)
	{
		const int32 MaxExp = GetExperienceLevel(20);
		PlayerStats.CurrentXP = FMath::Min(PlayerStats.CurrentXP, MaxExp);
	}
	
	if (ASurvivalPlayerController* SurvivalPC = GetSurvivalController())
	{
		if (Experience > 0)
		{
			SurvivalPC->UpdateExperienceUI(PlayerStats.CurrentXP, GetExperienceLevel(PlayerStats.CurrentLevel));
			SurvivalPC->ExperienceNotify(Experience);
		}

		if (bLevelUp)
		{
			SurvivalPC->LevelUpNotify(PlayerStats.CurrentLevel);
			SurvivalPC->UpdateLevelUp(PlayerStats.CurrentLevel);
			SurvivalPC->UpdateSkillPoints(PlayerStats.SkillPoints);
		}
	}
}

void ASurvivalCharacter::ServerAddExperience_Implementation(const int32 Experience)
{
	AddExperience(Experience);
}


void ASurvivalCharacter::OnAddExperience(const int32 Experience)
{
	if (HasAuthority())
	{
		AddExperience(Experience);
	}
	else
	{
		ServerAddExperience(Experience);
	}
}

void ASurvivalCharacter::ServerApplySkillPoints_Implementation(const EStatEnum& Stat)
{
	ApplySkillPoints(Stat);
}


void ASurvivalCharacter::ApplySkillPoints(const EStatEnum& Stat)
{
	if (PlayerStats.SkillPoints <= 0) return;
	float CurrentStat = 0.f;
	float MaxStat = 0.f;
	switch (Stat)
	{
	case EStatEnum::Health:
		PlayerStats.MaxHealth += 100.f;
		CurrentStat = PlayerStats.CurrentHealth;
		MaxStat = PlayerStats.MaxHealth;
		break;
	case EStatEnum::Food:
		PlayerStats.MaxFood += 100.f;
		CurrentStat = PlayerStats.CurrentFood;
		MaxStat = PlayerStats.MaxFood;
		break;
	case EStatEnum::Water:
		PlayerStats.MaxWater += 100.f;
		CurrentStat = PlayerStats.CurrentWater;
		MaxStat = PlayerStats.MaxWater;
		break;
	case EStatEnum::Stamina:
		PlayerStats.MaxStamina += 100.f;
		CurrentStat = PlayerStats.CurrentStamina;
		MaxStat = PlayerStats.MaxStamina;
		break;
	default:
		break;
	}
	PlayerStats.SkillPoints -= 1;
	if (ASurvivalPlayerController* SurvivalPC = GetSurvivalController())
	{
		SurvivalPC->UpdateStatBar(Stat, CurrentStat, MaxStat);
		SurvivalPC->UpdateSkillPoints(PlayerStats.SkillPoints);

		if (PlayerStats.SkillPoints <= 0)
		{
			SurvivalPC->RemoveLevelNotify();
		}
	}
}

// 接口实现
AController* ASurvivalCharacter::GetControllerFromChar_Implementation()
{
	return GetController();
}

APawn* ASurvivalCharacter::GetSurvivalCharRef_Implementation()
{
	return this;
}

void ASurvivalCharacter::ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage)
{
	MontageMulticast(ThirdPersonMontage);
}

void ASurvivalCharacter::FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage)
{
	ClientMontage(FirstPersonMontage);
}

FVector ASurvivalCharacter::GetArrowLocation_Implementation()
{
	const FVector ArrowLocation = Arrow1->GetComponentLocation();
	return ArrowLocation;
}

FRotator ASurvivalCharacter::GetArrowRotation_Implementation()
{
	const FRotator ArrowRotation = Arrow1->GetComponentRotation();
	return ArrowRotation;
}

int32 ASurvivalCharacter::GetTotalArmorPieces_Implementation()
{
	int32 ArmorSlots = 0;
	if (IsValid(HelmetSlots))
	{
		ArmorSlots++;
	}
	if (IsValid(ChestSlots))
	{
		ArmorSlots++;
	}
	if (IsValid(PantsSlots))
	{
		ArmorSlots++;
	}
	if (IsValid(BootsSlots))
	{
		ArmorSlots++;
	}
	return ArmorSlots;
}
