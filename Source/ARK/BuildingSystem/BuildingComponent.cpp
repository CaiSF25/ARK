// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingComponent.h"

#include "BuildableMaster.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

UBuildingComponent::UBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
	
	if (IsValid(BuildPreview))
	{
		SetPreviewColor(!CheckForOverlap());
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

void UBuildingComponent::SpawnBuildable(const FTransform& Transform)
{
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

void UBuildingComponent::SpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation)
{
	BuildTransform = Transform;

	if (BuildPlacementCheck(0, ClientCameraVector, ClientCameraRotation))
	{
		SpawnBuildable(Transform);
	}
	else
	{
		if (IsValid(BuildPreview))
		{
			BuildPreview->Destroy();
			BuildPreview = nullptr;
		}
	}
}

bool UBuildingComponent::CheckForOverlap() const
{
	FBuildableInfo BuildableInfo = BuildPreview->GetBuildableInfo();

	FVector StaticMeshOrigin;
	FVector StaticMeshBoxExtent;
	float StaticMeshSphereRadius;
	UKismetSystemLibrary::GetComponentBounds(BuildPreview->GetStaticMeshComponent(), StaticMeshOrigin, StaticMeshBoxExtent, StaticMeshSphereRadius);

	FVector OverlapBoxOrigin;
	FVector OverlapBoxExtent;
	float OverlapBoxSphereRadius;
	UKismetSystemLibrary::GetComponentBounds(BuildPreview->GetBoxComponent(), OverlapBoxOrigin, OverlapBoxExtent, OverlapBoxSphereRadius);

	FVector Start = BuildableInfo.UseCustomOverlap ? OverlapBoxOrigin : StaticMeshOrigin;
	FVector End = BuildableInfo.UseCustomOverlap ? OverlapBoxExtent : StaticMeshBoxExtent;
	constexpr float HalfSize = 1.2f;
	FRotator Rotation = FRotator(0, BuildPreview->GetActorRotation().Yaw + 90, 0);
	ETraceTypeQuery TraceQuery = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(BuildPreview);
	FHitResult OutHit;
	
	return UKismetSystemLibrary::BoxTraceSingle(
	   GetWorld(),
	   Start,
	   Start,
	   End / HalfSize,
	   Rotation,
	   TraceQuery,
	   false,
	   ActorsToIgnore,
	   EDrawDebugTrace::None,
	   OutHit,
	   true
	   );
}

bool UBuildingComponent::BuildPlacementCheck(const int32 StructureID, const FVector& ClientCameraVector,
	const FRotator& ClientCameraRotation)
{
	if (!BuildingClass) return false;

    // 获取 Class 的 CDO 来读取 mesh/box 的 bounds（不会实际 spawn actor）
    ABuildableMaster* CDO = Cast<ABuildableMaster>(BuildingClass->GetDefaultObject());
    if (!CDO) return false;

    // 尝试从 CDO 获取 box 或 mesh 组件的 bound
    FVector Origin;
    FVector BoxExtent;
    float SphereRadius = 0.f;
    UPrimitiveComponent* RelevantComp = nullptr;

    if (CDO->GetBoxComponent())
    {
        RelevantComp = CDO->GetBoxComponent();
    }
    else if (CDO->GetStaticMeshComponent())
    {
        RelevantComp = CDO->GetStaticMeshComponent();
    }

    if (!RelevantComp) return false;

    UKismetSystemLibrary::GetComponentBounds(RelevantComp, Origin, BoxExtent, SphereRadius);

    // 计算放置点（基于客户端传来的摄像机向量）
    constexpr int32 BuildDistanceClose = 350;
    constexpr int32 BuildDistanceFar = 1000;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return false;

    // 获取摄像机位置：使用传入的 ClientCameraVector/Rotation + 客户端的 CameraLocation
    // 这里你是把客户端的 CameraLocation 也传给服务器，或者用传入 Transform 的位置作为最终位置。
    // 假设你传入的是 Transform（Spawn 请求里），使用传入的 Transform 作为最终位置会更简单与安全。
    // 下面示例使用 ClientCameraVector 与传入的 Owner 的摄像机位置（若你传了摄像机位置，应以那个为准）
    // 这里假设调用方把 ClientCameraVector 与摄像机位置都传过来（例：ClientCameraVector、ClientCameraLocation）
    // 如果没有，请把必要值也传给 ServerSpawnBuild。

    // 计算 trace start/end（示例使用 Current Owner 的 camera location）
    // 你代码里传入了 ClientCameraVector 和 ClientCameraRotation，可以保持原逻辑：
    FVector CameraLocation;
    // 若你的 RPC 里没有摄像机位置，需要从 SurvivalCharacter 获取：
    if (GetOwner()->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
    {
        ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()));
        if (SurvivalCharacter && SurvivalCharacter->GetFirstPersonCamera())
        {
            CameraLocation = SurvivalCharacter->GetFirstPersonCamera()->GetComponentLocation();
        }
        else
        {
            CameraLocation = GetOwner()->GetActorLocation();
        }
    }

    FVector Start = CameraLocation + ClientCameraVector * BuildDistanceClose;
    FVector End   = CameraLocation + ClientCameraVector * BuildDistanceFar;

    // 使用 BoxTraceSingle：用 CDO 得到的 BoxExtent 作为 trace extents
    FHitResult OutHit;
    ETraceTypeQuery TraceQuery = UEngineTypes::ConvertToTraceType(ECC_Visibility);
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner()); // 忽略自身

    bool bHit = UKismetSystemLibrary::BoxTraceSingle(
        GetWorld(),
        Start,
        Start,                            // center - 你原先使用 Start,Start
        BoxExtent / 1.2f,                 // 你的 HalfSize 缩放
        FRotator(0.f, ClientCameraRotation.Yaw + 90.f, 0.f),
        TraceQuery,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        OutHit,
        true
    );

    // 如果 trace 有碰撞，认为不能放置
    return !bHit;
	
	/*SpawnBuildPreview(StructureID);
	if (!BuildPreview) return false;
	
	
	UWorld* World = GetWorld();
	if (!World) return false;
	
	if (!GetOwner() || !GetOwner()->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass())) return false;

	ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()));
	if (!SurvivalCharacter) return false;

	constexpr int32 BuildDistanceClose = 350;
	constexpr int32 BuildDistanceFar = 1000;

	UCameraComponent* Cam = SurvivalCharacter->GetFirstPersonCamera();
	if (!Cam) return false;
	
	FVector CameraLocation = Cam->GetComponentLocation();
	
	FVector Start = ClientCameraVector * BuildDistanceClose + CameraLocation;
	FVector End = ClientCameraVector * BuildDistanceFar + CameraLocation;

	FHitResult OutHit;

	FCollisionQueryParams Params;
	FCollisionQueryParams TraceParams;
	TArray<AActor*> IgnoreArray;
	TraceParams.AddIgnoredActors(IgnoreArray);

	bool bHit = World->LineTraceSingleByChannel(
			OutHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility,
			TraceParams
			);

	FVector DesiredLocation = bHit ? OutHit.ImpactPoint : End;
	FRotator DesiredRotation(0.f, ClientCameraRotation.Yaw + 90.f, 0.f);
	BuildTransform = FTransform(DesiredRotation, DesiredLocation);
	
	return !CheckForOverlap();*/
}

void UBuildingComponent::ServerSpawnBuild_Implementation(const FTransform& Transform, const FVector& ClientCameraVector,
                                                         const FRotator& ClientCameraRotation)
{
	SpawnBuild(Transform, ClientCameraVector, ClientCameraRotation);
}
