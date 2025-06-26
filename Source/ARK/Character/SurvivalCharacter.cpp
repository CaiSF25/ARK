// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ARK/Items/Equipables/FirstPersonEquipable.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogMyCharacter, Log, All);

ASurvivalCharacter::ASurvivalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 设置胶囊提大小
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCameraComponent"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(6.f, -2.5f, -160.f));

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(FirstPersonCameraComponent);
	Mesh3P->bCastDynamicShadow = false;
	Mesh3P->CastShadow = false;
	Mesh3P->SetRelativeLocation(FVector(6.f, -2.5f, -160.f));

	PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));

	PlayerHotBar = CreateDefaultSubobject<UPlayerHotBar>(TEXT("PlayerHotBar"));
}

void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FirstPersonInputContext, 0);
			Subsystem->AddMappingContext(UIInputContext, 1);
		}
	}
	if (Mesh3P)
	{
		if (UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &ASurvivalCharacter::OnThirdPersonMontageEnded);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASurvivalCharacter::OnThirdPersonNotifyBegin);
		}
	}
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASurvivalCharacter, EquipableState);
}


void ASurvivalCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

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
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Attack");
	if (HasAuthority())
	{
		if (IsValid(ThirdPersonEquippedItem))
		{
			if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
			{
				IEquipableItem::Execute_UseItemInterface(ThirdPersonEquippedItem, this);
			}
		}
	}
	else
	{
		ServerAttack();
	}
}

void ASurvivalCharacter::InteractPressed()
{
}

void ASurvivalCharacter::HotbarPressed(int32 Index)
{
	ServerHotbar(Index);
}

void ASurvivalCharacter::OnThirdPersonMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "OnThirdPersonMontageEnded");
	if (IsValid(ThirdPersonEquippedItem))
	{
		if (ThirdPersonEquippedItem->GetClass()->ImplementsInterface(UEquipableItem::StaticClass()))
		{
			IEquipableItem::Execute_EndAnimation(ThirdPersonEquippedItem);
		}
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

void ASurvivalCharacter::ServerOnSlotDrop_Implementation(
	EContainerType FromContainer,
	EContainerType TargetContainer,
	int32 FromIndex,
	int32 DroppedIndex,
	EArmorType ArmorType)
{
	PlayerInventory->ServerOnSlotDrop(PlayerInventory, FromIndex, DroppedIndex);
}


ASurvivalPlayerController* ASurvivalCharacter::GetControllerFromChar_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Survival Character");
	return Cast<ASurvivalPlayerController>(GetController());
}


void ASurvivalCharacter::SpawnEquipableThirdPerson_Implementation(TSubclassOf<AActor> Class, FItemInfo ItemInfo,
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
				SpawnEquipableThirdPerson(EquippedItemInfo.ItemClassRef, EquippedItemInfo, HorbarIndex);
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

void ASurvivalCharacter::ClientMontage_Implementation(UAnimMontage* FirstPersonMontage)
{
	if (!Mesh1P || !FirstPersonMontage) return;
	if (UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance())
	{
		AnimInstance->Montage_Play(FirstPersonMontage);
	}
}

void ASurvivalCharacter::ThirdPersonMontage_Implementation(UAnimMontage* ThirdPersonMontage)
{
	MontageMulticast(ThirdPersonMontage);
}

void ASurvivalCharacter::FirstPersonMontage_Implementation(UAnimMontage* FirstPersonMontage)
{
	ClientMontage(FirstPersonMontage);
}


void ASurvivalCharacter::MontageMulticast_Implementation(UAnimMontage* ThirdPersonMontage)
{
	if (!Mesh3P || !ThirdPersonMontage) return;
	if (UAnimInstance* AnimInstance = Mesh3P->GetAnimInstance())
	{
		AnimInstance->Montage_Play(ThirdPersonMontage);
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

void ASurvivalCharacter::ServerHotbar_Implementation(int32 Index)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Hotbar");
	UseHotbarFunction(Index);
}


class ASurvivalCharacter* ASurvivalCharacter::GetSurvivalCharRef_Implementation()
{
	return this;
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

		EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Triggered, this, &ASurvivalCharacter::InteractPressed);

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

