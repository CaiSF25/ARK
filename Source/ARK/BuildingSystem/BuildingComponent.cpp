
// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingComponent.h"

#include "BuildableMaster.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "Camera/CameraComponent.h"
#include "Chaos/Utilities.h"
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

void UBuildingComponent::BuildMode(const int32 StructureID)
{
	const UCameraComponent* CameraComponent = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()))->GetFirstPersonCamera();
	if (!CameraComponent) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsValid(BuildPreview))
	{
		SpawnBuildPreview(StructureID);
	}

	constexpr int32 BuildDistanceClose = 350;
	constexpr int32 BuildDistanceFar = 1000;

	FVector Forward = CameraComponent->GetForwardVector();
	FVector Location = CameraComponent->GetComponentLocation();
	FRotator Rotation = CameraComponent->GetComponentRotation();
	
	FVector Start = Forward * BuildDistanceClose + Location;
	FVector End = Forward * BuildDistanceFar + Location;
	
	FHitResult OutHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(BuildPreview);
	
	bool bHit = World->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		BuildPreview->GetBuildableInfo().TraceChannel,
		TraceParams
		);

	if (bHit)
	{
		HitActor = OutHit.GetActor();
		HitComponent = OutHit.GetComponent();
	}

	FVector DesiredLocation = bHit ? OutHit.ImpactPoint : End;
	FRotator DesiredRotation(0.f, Rotation.Yaw + 90.f, 0.f);
	BuildTransform = FTransform(DesiredRotation, DesiredLocation);
	
	if (IsValid(BuildPreview))
	{
		std::pair<bool, FTransform> Result = GetSnappingPoints();
		if (Result.first)
		{
			BuildTransform = Result.second;
		}
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
	// if (!OwnerPawn) return;
	// if (!OwnerPawn->IsLocallyControlled()) return;

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cant Building");
	}
	if (IsValid(BuildPreview))
	{
		BuildPreview->Destroy();
		BuildPreview = nullptr;
	}
}

bool UBuildingComponent::CheckForOverlap() const
{
	if (!IsValid(BuildPreview))
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckForOverlap: BuildPreview invalid"));
		return true; // 保守处理：有问题时当做阻挡
	}
	
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
	/*UWorld* World = GetWorld();
    if (!World) 
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: No World"));
        return false;
    }

    if (!GetOwner() || !GetOwner()->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: Owner invalid or doesn't implement interface"));
        return false;
    }

    ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()));
    if (!SurvivalCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: SurvivalCharacter cast failed"));
        return false;
    }

    UCameraComponent* FirstPersonCamera = SurvivalCharacter->GetFirstPersonCamera();
    if (!FirstPersonCamera) 
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: No camera"));
        return false;
    }

    FVector CameraLocation = FirstPersonCamera->GetComponentLocation();

    const float BuildDistanceClose = 350.f;
    const float BuildDistanceFar = 1000.f;
	
    FVector Forward = ClientCameraVector.GetSafeNormal();
    if (Forward.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: ClientCameraVector is zero"));
        return false;
    }

    FVector StartLocation = CameraLocation + Forward * BuildDistanceClose;
    FVector EndLocation = CameraLocation + Forward * BuildDistanceFar;
	
    if (!IsValid(BuildPreview))
    {
        SpawnBuildPreview(StructureID);
        if (!IsValid(BuildPreview))
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnBuildPreview failed"));
            return false;
        }
    	
        BuildPreview->SetActorEnableCollision(false);
    }
	
    ECollisionChannel TraceChannel = ECC_WorldStatic; // 默认
    if (IsValid(BuildPreview))
    {
        TraceChannel = BuildPreview->GetBuildableInfo().TraceChannel;
    }
	
    FHitResult OutHit;
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(BuildPlacementTrace), true);
    TraceParams.AddIgnoredActor(GetOwner());

    bool bHit = World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannel, TraceParams);

    FVector DesiredLocation = bHit ? OutHit.ImpactPoint : EndLocation;
    FRotator DesiredRotation(0.f, ClientCameraRotation.Yaw + 90.f, 0.f);
    BuildTransform = FTransform(DesiredRotation, DesiredLocation);

    if (IsValid(BuildPreview))
    {
        BuildPreview->SetActorTransform(BuildTransform);
    }

    if (bHit && IsValid(BuildPreview))
    {
        return !CheckForOverlap();
    }

    return false;*/
	
	UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: No World"));
        return false;
    }

    if (!GetOwner() || !GetOwner()->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: Owner invalid or doesn't implement interface"));
        return false;
    }

    ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()));
    if (!SurvivalCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: SurvivalCharacter cast failed"));
        return false;
    }

    UCameraComponent* FirstPersonCamera = SurvivalCharacter->GetFirstPersonCamera();
    if (!FirstPersonCamera)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: No camera"));
        return false;
    }

    FVector CameraLocation = FirstPersonCamera->GetComponentLocation();

    const float BuildDistanceClose = 350.f;
    const float BuildDistanceFar = 1000.f;

    FVector Forward = ClientCameraVector.GetSafeNormal();
    if (Forward.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck: ClientCameraVector is zero"));
        return false;
    }

    FVector StartLocation = CameraLocation + Forward * BuildDistanceClose;
    FVector EndLocation   = CameraLocation + Forward * BuildDistanceFar;

    // Decide TraceChannel (preview preference, else CDO)
    ECollisionChannel TraceChannel = ECC_WorldStatic;
    if (IsValid(BuildPreview))
    {
        TraceChannel = BuildPreview->GetBuildableInfo().TraceChannel;
    }
    else if (BuildingClass)
    {
        if (ABuildableMaster* CDO = Cast<ABuildableMaster>(BuildingClass->GetDefaultObject()))
        {
            TraceChannel = CDO->GetBuildableInfo().TraceChannel;
        }
    }

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
    {
        const FString ChannelName = StaticEnum<ECollisionChannel>()->GetNameStringByValue((int64)TraceChannel);
        UE_LOG(LogTemp, Log, TEXT("Build: Using TraceChannel = %s (%d)"), *ChannelName, (int32)TraceChannel);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
                FString::Printf(TEXT("TraceChannel: %s (%d)"), *ChannelName, (int32)TraceChannel));
        }
    }
#endif

    // Line trace to determine impact point / potential snap target
    FHitResult OutHit;
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(BuildPlacementTrace), true);
    TraceParams.AddIgnoredActor(GetOwner());
    bool bHit = World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannel, TraceParams);

    FVector DesiredLocation = bHit ? OutHit.ImpactPoint : EndLocation;
    FRotator DesiredRotation(0.f, ClientCameraRotation.Yaw + 90.f, 0.f);
    BuildTransform = FTransform(DesiredRotation, DesiredLocation);

    if (bHit)
    {
        HitActor = OutHit.GetActor();
        HitComponent = OutHit.GetComponent();
    }

    if (IsValid(BuildPreview))
    {
        BuildPreview->SetActorTransform(BuildTransform);
    }

    // overlap/box check
    bool bCanPlace = false;
    if (IsValid(BuildPreview))
    {
        bCanPlace = !CheckForOverlap();
        return bCanPlace;
    }

    // No preview: server authoritative check using CDO bounds and sweep
    if (!GetOwner()->HasAuthority())
    {
        // non-authority and no preview -> cannot safely decide
        return false;
    }

    if (!BuildingClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck (Server): No BuildingClass"));
        return false;
    }

    ABuildableMaster* CDO = Cast<ABuildableMaster>(BuildingClass->GetDefaultObject());
    if (!CDO)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck (Server): CDO null"));
        return false;
    }

    // select component to base bounds on
    UPrimitiveComponent* RelevantComp = nullptr;
    if (CDO->GetBoxComponent()) RelevantComp = CDO->GetBoxComponent();
    else if (CDO->GetStaticMeshComponent()) RelevantComp = CDO->GetStaticMeshComponent();

    if (!RelevantComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildPlacementCheck (Server): CDO has no relevant component"));
        return false;
    }

    FVector LocalOrigin; FVector LocalExtent; float LocalSphereRadius;
    UKismetSystemLibrary::GetComponentBounds(RelevantComp, LocalOrigin, LocalExtent, LocalSphereRadius);

    if (LocalExtent.IsNearlyZero(0.001f))
    {
        if (UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(RelevantComp))
        {
            if (SMC->GetStaticMesh())
            {
                const FBoxSphereBounds MeshBounds = SMC->GetStaticMesh()->GetBounds();
                LocalExtent = MeshBounds.BoxExtent * SMC->GetRelativeScale3D();
            }
        }
        if (LocalExtent.IsNearlyZero(0.001f))
        {
            LocalExtent = FVector(50.f, 50.f, 50.f);
        }
        LocalOrigin = RelevantComp->GetRelativeLocation();
    }

    const FRotator BoxRotation(0.f, BuildTransform.Rotator().Yaw + 90.f, 0.f);
    constexpr float HalfSizeFactor = 1.2f;
    const FVector QueryExtent = LocalExtent / HalfSizeFactor;

    const FVector WorldBoxCenter = BuildTransform.TransformPosition(LocalOrigin);

    ECollisionChannel ServerTraceChannel = TraceChannel;

    // ignored actors
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ServerBuildBoxTrace), false);
    if (AActor* OwnerActor = GetOwner())
    {
        QueryParams.AddIgnoredActor(OwnerActor);
        if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
        {
            QueryParams.AddIgnoredActor(OwnerPawn);
        }
    }
    if (IsValid(BuildPreview))
    {
        QueryParams.AddIgnoredActor(BuildPreview);
    }

    FCollisionShape BoxShape = FCollisionShape::MakeBox(QueryExtent);

    // First sweep at candidate transform
    FHitResult HitOut;
    bool bHitBox = false;
    if (UWorld* WorldPtr = GetWorld())
    {
        bHitBox = WorldPtr->SweepSingleByChannel(
            HitOut,
            WorldBoxCenter,
            WorldBoxCenter,
            BoxRotation.Quaternion(),
            ServerTraceChannel,
            BoxShape,
            QueryParams
        );
    }

    if (!bHitBox)
    {
        // no blocking hit -> can place
        return true;
    }

    // We hit something. If it's a buildable (implements BuildInterface) -> attempt server-side snap
    AActor* BlockActor = HitOut.GetActor();
    UPrimitiveComponent* BlockComp = HitOut.GetComponent();

    UE_LOG(LogTemp, Warning, TEXT("[Build][Server] BoxTrace BLOCKED by Actor=%s, Component=%s, Location=%s"),
           *GetNameSafe(BlockActor),
           *GetNameSafe(BlockComp),
           *HitOut.ImpactPoint.ToString());

    if (BlockComp)
    {
        const FName ProfileName = BlockComp->GetCollisionProfileName();
        const ECollisionChannel ObjType = BlockComp->GetCollisionObjectType();
        const ECollisionResponse ResponseToChannel = BlockComp->GetCollisionResponseToChannel(ServerTraceChannel);

        UE_LOG(LogTemp, Warning, TEXT("[Build][Server] BlockComp Profile=%s, ObjectType=%d, ResponseToServerChannel=%d"),
               *ProfileName.ToString(), (int32)ObjType, (int32)ResponseToChannel);
    }

    // If the hit actor is a buildable, try snapping
    if (BlockActor && BlockActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Log, TEXT("[Build][Server] Hit buildable %s - attempting server-side snap"), *GetNameSafe(BlockActor));

        // set member HitActor/HitComponent so GetSnappingPoints() can use them
        this->HitActor = BlockActor;
        this->HitComponent = BlockComp;

        auto SnapResult = GetSnappingPoints();
        // clear members after call (we'll set back if needed)
        this->HitActor = nullptr;
        this->HitComponent = nullptr;

        if (SnapResult.first)
        {
            // found snap transform on server -> update BuildTransform and re-check collisions
            BuildTransform = SnapResult.second;

            // recompute world center with snapped transform
            const FVector SnappedWorldCenter = BuildTransform.TransformPosition(LocalOrigin);

            // re-run sweep at snapped transform
            FHitResult HitAfterSnap;
            bool bHitAfterSnap = false;
            if (UWorld* WorldPtr2 = GetWorld())
            {
                bHitAfterSnap = WorldPtr2->SweepSingleByChannel(
                    HitAfterSnap,
                    SnappedWorldCenter,
                    SnappedWorldCenter,
                    BoxRotation.Quaternion(),
                    ServerTraceChannel,
                    BoxShape,
                    QueryParams
                );
            }

            if (!bHitAfterSnap)
            {
                UE_LOG(LogTemp, Log, TEXT("[Build][Server] Snap succeeded and no conflicts after snap -> allow placement"));
                return true;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[Build][Server] Snap succeeded but still conflicts at snapped position -> deny"));
                // optionally: log HitAfterSnap info for debugging
                UE_LOG(LogTemp, Warning, TEXT("[Build][Server] AfterSnap blocked by Actor=%s Comp=%s Loc=%s"),
                       *GetNameSafe(HitAfterSnap.GetActor()),
                       *GetNameSafe(HitAfterSnap.GetComponent()),
                       *HitAfterSnap.ImpactPoint.ToString());
                return false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[Build][Server] Snap attempt failed for %s"), *GetNameSafe(BlockActor));
            return false;
        }
    }

    // not a buildable we can snap to -> deny placement
    return false;
}

std::pair<bool, FTransform> UBuildingComponent::GetSnappingPoints()
{
	bool bFound = false;
	if (!IsValid(HitActor) || !HitComponent) return {bFound, FTransform()};
	if (!HitActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass())) return {bFound, FTransform()};
	TArray<UBoxComponent*> Boxes = IBuildInterface::Execute_GetBoxes(HitActor);

	
	for (UBoxComponent* Box : Boxes)
	{
		if (!Box) continue;
		if (HitComponent == Box)
		{
			bFound = true;
			// 没有找到HitActor
			break;
		}
	}
	return {bFound, HitComponent->GetComponentTransform()};
}

void UBuildingComponent::ServerSpawnBuild_Implementation(const FTransform& Transform, const FVector& ClientCameraVector,
                                                         const FRotator& ClientCameraRotation)
{
	SpawnBuild(Transform, ClientCameraVector, ClientCameraRotation);
}
