// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWindow.h"
#include "MovieSceneTracksComponentTypes.h"
#include "ARK/Character/SurvivalPlayerController.h"
#include "ARK/Interfaces/ArmorItemInterface.h"
#include "ARK/Items/ArmorMaster.h"
#include "ARK/Items/Equipables/FirstPersonEquipable.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
APlayerWindow::APlayerWindow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(Root);
	Capsule->InitCapsuleSize(125.f, 125.f);
	Capsule->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));
	Capsule->SetCollisionProfileName(TEXT("BlockAll"));

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Capsule);
	SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -125.0f));

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SceneCaptureComponent2D->SetupAttachment(Capsule);
	SceneCaptureComponent2D->SetRelativeLocation(FVector(4.0f, 160.0f, -15.f));
	SceneCaptureComponent2D->SetRelativeRotation(FRotator(-5.0f, -90.0f, 0.0f));
	SceneCaptureComponent2D->FOVAngle = 50.f;
	SceneCaptureComponent2D->MaxViewDistanceOverride = 200.f;
}

void APlayerWindow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void APlayerWindow::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerWindow::InitialWindow_Implementation(ASurvivalPlayerController* SurvivalPC)
{
	UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 500, 800);
	SceneCaptureComponent2D->TextureTarget = RenderTarget;
	
	DynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	DynMat->SetTextureParameterValue(FName("Param"), RenderTarget);
	if (!IsValid(SurvivalPC)) return;
	SurvivalPC->SetRenderMaterial(DynMat);
}

void APlayerWindow::EquipItem(const FEquipableInfo& Info)
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	EquippedState = Info.AnimationState;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Spawned = World->SpawnActor<AActor>(Info.FirstPersonEquipClass, FTransform::Identity, SpawnParams);
	if (!IsValid(Spawned)) return;

	Spawned->SetReplicates(false);
	Spawned->SetReplicateMovement(false);

	Spawned->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Info.SocketName);
	EquippedItemRef = Spawned;
}

void APlayerWindow::DequipItem()
{
	if (IsValid(EquippedItemRef))
	{
		EquippedItemRef->Destroy();
		EquippedState = EEquipableState::Default;
	}
}

void APlayerWindow::EquipArmor(const FItemInfo& ItemInfo)
{
	if (!IsValid(ItemInfo.ItemClassRef)) return;
	
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	const EArmorType ArmorType = ItemInfo.ArmorType;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AItemMaster* Spawned = World->SpawnActor<AItemMaster>(ItemInfo.ItemClassRef, FTransform::Identity, SpawnParams);

	if (!IsValid(Spawned)) return;

	Spawned->SetReplicates(false);
	Spawned->SetReplicateMovement(false);

	if (IsValid(SkeletalMesh) && Spawned->GetRootComponent())
	{
		const FName SocketName = NAME_None;
		Spawned->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}
	
	if (Spawned->GetClass()->ImplementsInterface(UArmorItemInterface::StaticClass()))
	{
		if (const AActor* ArmorRef = IArmorItemInterface::Execute_GetArmorRef(Spawned); IsValid(ArmorRef))
		{
			if (USkeletalMeshComponent* ArmorSkel = ArmorRef->FindComponentByClass<USkeletalMeshComponent>(); IsValid(ArmorSkel) && IsValid(SkeletalMesh))
			{
				ArmorSkel->SetLeaderPoseComponent(SkeletalMesh);
			}
		}
	}

	AItemMaster*& TargetSlot = [&]() -> AItemMaster*&
	{
		switch (ItemInfo.ArmorType)
		{
		case EArmorType::Helmet: return HelmetSlot;
		case EArmorType::Chest:  return ChestSlot;
		case EArmorType::Pants:  return PantsSlot;
		case EArmorType::Boots:   return BootsSlot;
		default: return HelmetSlot;
		}
	}();

	if (IsValid(TargetSlot))
	{
		TargetSlot->Destroy();
		TargetSlot = nullptr;
	}

	TargetSlot = Spawned;
}

void APlayerWindow::DequipArmor(const EArmorType& ArmorType)
{
	AItemMaster*& TargetSlot = [&]() -> AItemMaster*&
	{
		switch (ArmorType)
		{
		case EArmorType::Helmet: return HelmetSlot;
		case EArmorType::Chest:  return ChestSlot;
		case EArmorType::Pants:  return PantsSlot;
		case EArmorType::Boots:   return BootsSlot;
		default: return HelmetSlot;
		}
	}();

	if (IsValid(TargetSlot))
	{
		TargetSlot->Destroy();
		TargetSlot = nullptr;
	}
}

APlayerWindow* APlayerWindow::GetPlayerWindowRef_Implementation()
{
	return this;
}

