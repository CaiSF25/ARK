// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ARK/HarvestingSystem/DestructableHarvestable.h"
#include "ARK/HarvestingSystem/GroundItemMaster.h"
#include "ARK/HarvestingSystem/LargeItem.h"
#include "ARK/Interfaces/GroundItemInterface.h"
#include "ARK/Items/Equipables/FirstPersonEquipable.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

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
	DataTable = CreateDefaultSubobject<UDataTable>(TEXT("DataTable"));
	GroundResourcesTable = CreateDefaultSubobject<UDataTable>(TEXT("GroundResources"));

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
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASurvivalCharacter, EquipableState);
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

void ASurvivalCharacter::OnSpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo,
	int32 LocalEquippedIndex)
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

void ASurvivalCharacter::SpawnEquipableThirdPerson(TSubclassOf<AActor> Class, FItemInfo ItemInfo,
                                                   int32 LocalEquippedIndex)
{
	EquippedIndex = LocalEquippedIndex;
	UWorld* World = GetWorld();
	if (World)
	{
		ThirdPersonEquippedItem = World->SpawnActor(Class);
		ThirdPersonEquippedItem->SetOwner(this);
		if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			AEquipableMaster* EquipRef = IEquipableItem::Execute_GetEquipableRef(ThirdPersonEquippedItem);
			MulticastWeaponEquip(ThirdPersonEquippedItem, EquipRef->EquipableInfo.SocketName, EquipRef->EquipableInfo.AnimationState);
			SpawnEquipableFirstPerson(EquipRef->EquipableInfo.FirstPersonEquipClass, EquipRef->EquipableInfo.SocketName);
		}
	}
}

void ASurvivalCharacter::OverlapGroundItems()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	const TSubclassOf<AActor> ClassFilter = nullptr;

	const TArray<AActor*> ActorsToIgnore = { this };

	TArray<AActor*> OutActors;
	
	if (UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation(),
		70.f,
		ObjectTypes,
		ClassFilter,
		ActorsToIgnore, 
		OutActors
		))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "We Hit a Bush");
		HarvestGroundItem(OutActors[0]);
		OnHarvestMontage();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "No Hit");
	}

	const UWorld* World = GetWorld();
	DrawDebugSphere(
		World,
		GetActorLocation(),
		70.f,
		12,
		FColor::Red,
		false,
		3,
		0,
		2
		);
}

void ASurvivalCharacter::SpawnEquipableFirstPerson_Implementation(TSubclassOf<AActor> Class, FName SocketName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FirstPersonEquippedItem = World->SpawnActor(Class);
		const FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget,    // Location
				EAttachmentRule::SnapToTarget,    // Rotation
				EAttachmentRule::SnapToTarget,    // Scale
				true                              // Weld simulated bodies?
				);
		FirstPersonEquippedItem->AttachToComponent(Mesh1P, AttachmentRules, SocketName);
	}
}


void ASurvivalCharacter::UseHotbarFunction(int32 Index)
{
	HorbarIndex = Index;
	const EItemType ItemType = PlayerHotBar->CheckHotbar(Index);
	if (ItemType != EItemType::None)
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
				FItemInfo EquippedItemInfo = PlayerHotBar->GetItems()[HorbarIndex];
				OnSpawnEquipableThirdPerson(EquippedItemInfo.ItemClassRef, EquippedItemInfo, HorbarIndex);
			}
			break;
		case EItemType::Armor:
			break;
		case EItemType::Consumable:
			break;
		case EItemType::Buildable:
			break;
			
		default:
			break;
		
					
		}
	}
}

void ASurvivalCharacter::Hotbar(int32 Index)
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

void ASurvivalCharacter::OnHarvestMontage()
{
	if (HasAuthority())
	{
		if (!bIsHarvesting)
		{
			bIsHarvesting = true;
			if (UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance())
			{
				AnimInstance->Montage_Play(PickUpMontage);
				ClientMontage(PickUpMontage);
				MontageMulticast(PickUpMontage);
			}
		}
	}
	else
	{
		ServerHarvestMontage();
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

void ASurvivalCharacter::MulticastWeaponEquip_Implementation(AActor* Target, FName SocketName,
                                                             const EEquipableState& EquippedState)
{
	if (IsValid(Target))
	{
		if (IsValid(Mesh3P))
		{
			const FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget,    // Location
				EAttachmentRule::SnapToTarget,    // Rotation
				EAttachmentRule::SnapToTarget,    // Scale
				true                              // Weld simulated bodies?
				);
			Target->AttachToComponent(Mesh3P, AttachmentRules, SocketName);
			EquipableState = EquippedState;
		}
	}
}

void ASurvivalCharacter::DequipThirdPerson_Implementation()
{
	EquipableState = EEquipableState::Default;
}

void ASurvivalCharacter::DequipFirstPerson_Implementation()
{
	FirstPersonEquippedItem->Destroy();
}

void ASurvivalCharacter::DequipCurItem(int32 Index)
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


void ASurvivalCharacter::ServerHotbar_Implementation(int32 Index)
{
	UseHotbarFunction(Index);
}

void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	FItemInfo* Row = DataTable->FindRow<FItemInfo>(
		RowName,
		ContextString,
		true
		);
	if (Row)
	{
		Row->ItemQuantity = Resource.Quantity;
		PlayerInventory->ServerAddItem(*Row);
		ASurvivalPlayerController* PlayerController = Cast<ASurvivalPlayerController>(GetController());
		PlayerController->ShowItemWidget(Row->ItemIcon, Row->ItemQuantity, Row->ItemName);
	}
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
			
			const float LocalHealth = GroundItem->GetHealth() - 15;
			if (LocalHealth <= 0)
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

void ASurvivalCharacter::HandleSlotDrop(EContainerType FromContainer, EContainerType TargetContainer, int32 FromIndex, int32 DroppedIndex, EArmorType ArmorType)
{
	switch (TargetContainer)
	{
	case EContainerType::PlayerInventory:
		switch (FromContainer)
		{
	case EContainerType::PlayerInventory:
		PlayerInventory->ServerOnSlotDrop(PlayerInventory, FromIndex, DroppedIndex);
			break;
	case EContainerType::PlayerHotbar:
		PlayerInventory->ServerOnSlotDrop(PlayerHotBar, FromIndex, DroppedIndex);
			break;
		}
		break;

	case EContainerType::PlayerHotbar:
		switch (FromContainer)
		{
	case EContainerType::PlayerInventory:
		PlayerHotBar->ServerOnSlotDrop(PlayerInventory, FromIndex, DroppedIndex);
			break;
	case EContainerType::PlayerHotbar:
		PlayerHotBar->ServerOnSlotDrop(PlayerHotBar, FromIndex, DroppedIndex);
			break;
		}
		break;
		

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
	if (!bIsHarvesting)
	{
		bIsHarvesting = true;
		if (UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance())
		{
			AnimInstance->Montage_Play(PickUpMontage);
			ClientMontage(PickUpMontage);
			MontageMulticast(PickUpMontage);
		}
	}
}

// 接口实现
ASurvivalPlayerController* ASurvivalCharacter::GetControllerFromChar_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Survival Character");
	return Cast<ASurvivalPlayerController>(GetController());
}

ASurvivalCharacter* ASurvivalCharacter::GetSurvivalCharRef_Implementation()
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

