// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingComponent.h"

#include "BuildableMaster.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UBuildingComponent::UBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBuildingComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuildingComponent, bIsBuildModeEnabled);
}


// Called when the game starts
void UBuildingComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuildingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopBuildLoop();
	
	Super::EndPlay(EndPlayReason);
}

void UBuildingComponent::OnSpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector,
	const FRotator& ClientCameraRotation)
{
	if (GetOwner()->HasAuthority())
	{
		SpawnBuild(Transform, ClientCameraVector, ClientCameraRotation);
	}
	else
	{
		ServerSpawnBuild(Transform, ClientCameraVector, ClientCameraRotation);
	}
}

void UBuildingComponent::OnRep_BuildMode()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	// 仅对本地玩家启动/停止 preview loop（别的客户端不需要运行本地预览）
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	if (bIsBuildModeEnabled)
	{
		// 使用服务器复制回来的 StructureID 或本地已有 ID
		BuildMode(ReplicatedStructureID);
	}
	else
	{
		StopBuildLoop();
	}
}

void UBuildingComponent::ClientLaunchBuildMode_Implementation(int32 StructureID)
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()); OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		if (bIsBuildModeEnabled)
		{
			StopBuildLoop();
		}
		else
		{
			bIsBuildModeEnabled = true;
			BuildMode(StructureID);
		}
	}
}

void UBuildingComponent::BuildMode(const int32 StructureID)
{
	const UCameraComponent* CameraComponent = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()))->GetFirstPersonCamera();
	if (!CameraComponent) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	constexpr int32 BuildDistanceClose = 350;
	constexpr int32 BuildDistanceFar = 1000;

	FVector Forward = CameraComponent->GetForwardVector();
	FVector Location = CameraComponent->GetComponentLocation();
	FRotator Rotation = CameraComponent->GetComponentRotation();
	
	FVector Start = Forward * BuildDistanceClose + Location;
	FVector End = Forward * BuildDistanceFar + Location;
	
	FHitResult OutHit;
	FCollisionQueryParams TraceParams;
	TArray<AActor*> IgnoreArray;
	TraceParams.AddIgnoredActors(IgnoreArray);
	
	bool bHit = World->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		ECC_Visibility,
		TraceParams
		);

	FVector DesiredLocation = bHit ? OutHit.ImpactPoint : End;
	FRotator DesiredRotation(0.f, Rotation.Yaw + 90.f, 0.f);
	BuildTransform = FTransform(DesiredRotation, DesiredLocation);
	
	if (!IsValid(BuildPreview))
	{
		SpawnBuildPreview(StructureID);
	}

	if (IsValid(BuildPreview))
	{
		SetPreviewColor(false); 
	}
	else
	{
		SpawnBuildPreview(StructureID);
	}

	StartBuildLoop(StructureID);
}


void UBuildingComponent::SpawnBuildPreview(int32 StructureID)
{
	UWorld* World = GetWorld();
	if (!World) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	if (!OwnerPawn->IsLocallyControlled()) return;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = OwnerPawn;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ABuildableMaster* Preview = World->SpawnActor<ABuildableMaster>(BuildingClass, BuildTransform, Params);
	if (!Preview) return;

	Preview->SetReplicates(false);
	Preview->SetReplicateMovement(false);

	Preview->SetActorEnableCollision(false);

	BuildPreview = Preview;
}

void UBuildingComponent::StartBuildLoop(int32 StructureID)
{
	if (bIsBuildModeEnabled)
	{ 
		const FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UBuildingComponent::BuildMode, StructureID);;
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				BuildLoopTimerHandle,
				TimerDel,
				0.0167,
				true
				);
		}
	}
	else
	{
		StopBuildLoop();
	}
}

void UBuildingComponent::StopBuildLoop()
{
	bIsBuildModeEnabled = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BuildLoopTimerHandle);
	}

	if (IsValid(BuildPreview))
	{
		BuildPreview->Destroy();
		BuildPreview = nullptr;
	}
}

void UBuildingComponent::SetPreviewColor(const bool bCanPlace) const
{
	if (!GreenMaterial) return;
	if (!RedMaterial) return;
	
	TArray<UStaticMeshComponent*> MeshComps;
	BuildPreview->GetComponents<UStaticMeshComponent>(MeshComps);

	for (const auto& MeshComp : MeshComps)
	{
		for (int32 Index = 0; Index < MeshComp->GetNumMaterials(); Index++)
		{
			if (bCanPlace)
			{
				MeshComp->SetMaterial(Index, GreenMaterial);
			}
			else
			{
				MeshComp->SetMaterial(Index, RedMaterial);
			}
		}
	}
	BuildPreview->SetActorTransform(BuildTransform);
}

void UBuildingComponent::SpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SpawnBuild");
	BuildTransform = Transform;
	
	UWorld* World = GetWorld();
	if (!World) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = OwnerPawn;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ABuildableMaster* Preview = World->SpawnActor<ABuildableMaster>(BuildingClass, Transform, Params);
	if (!Preview) return;

	Preview->SetReplicates(true);
	Preview->SetReplicateMovement(false);

	Preview->SetActorEnableCollision(true);
}

void UBuildingComponent::ServerSpawnBuild_Implementation(const FTransform& Transform, const FVector& ClientCameraVector,
	const FRotator& ClientCameraRotation)
{
	SpawnBuild(Transform, ClientCameraVector, ClientCameraRotation);
}
