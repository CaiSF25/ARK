
// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingComponent.h"
#include "BuildableMaster.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBuildingComponent, Log, All);

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

	if (const ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner())))
	{
		SurvivalCamera = SurvivalCharacter->GetFirstPersonCamera();
	}
}

void UBuildingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopBuildLoop();
	
	Super::EndPlay(EndPlayReason);
}

void UBuildingComponent::OnSpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector,
	const FRotator& ClientCameraRotation, int32 StructureIDTemp)
{
	if (GetOwner()->HasAuthority())
	{
		SpawnBuild(Transform, ClientCameraVector, ClientCameraRotation, StructureIDTemp);
	}
	else
	{
		ServerSpawnBuild(Transform, ClientCameraVector, ClientCameraRotation, StructureIDTemp);
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
		BuildMode(StructureIDDebug);
	}
}

void UBuildingComponent::BuildMode(const int32 StructureID)
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(SurvivalCamera)) return;

	if (!IsValid(BuildPreview))
	{
		SpawnBuildPreview(StructureID);
		if (!IsValid(BuildPreview)) return;
		BuildPreview->SetActorEnableCollision(false);
	}

	// 清除碰撞指针
	HitActor = nullptr;
	HitComponent = nullptr;

	constexpr int32 BuildDistanceClose = 350;
	constexpr int32 BuildDistanceFar = 1000;

	FVector Forward = SurvivalCamera->GetForwardVector().GetSafeNormal();
	if (Forward.IsNearlyZero()) return;
	
	FVector Location = SurvivalCamera->GetComponentLocation();
	FRotator Rotation = SurvivalCamera->GetComponentRotation();
	
	FVector Start = Forward * BuildDistanceClose + Location;
	FVector End = Forward * BuildDistanceFar + Location;
	
	FHitResult OutHit;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(BuildPreviewTrace), true);
	if (IsValid(BuildPreview)) TraceParams.AddIgnoredActor(BuildPreview);
	
	const bool bHit = World->LineTraceSingleByChannel(
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
	
	std::pair<bool, FTransform> Result = GetSnappingPoints();
	const bool bSnapped = Result.first;
	if (bSnapped)
	{
		BuildTransform = Result.second;
	}
	const FBuildableInfo BuildInfo = BuildPreview->GetBuildableInfo();
    const bool bDoFloatCheck = BuildInfo.DoFloatCheck;
	
    const bool bOverlap = CheckForOverlap();
	
    const bool bFloating = IsBuildFloating();
	
    bool bCanPlace = false;
    if (bSnapped)
    {
        if (!bDoFloatCheck)
        {
            bCanPlace = !bOverlap && bHit;
        }
        else
        {
            bCanPlace = !bOverlap && bHit && bFloating;
        }
    }
    else
    {
        bCanPlace = !bOverlap && bHit && bFloating;
    }
	
    SetPreviewColor(bCanPlace);
	
    StartBuildLoop(StructureID);
}


void UBuildingComponent::SpawnBuildPreview(int32 StructureID)
{
	UWorld* World = GetWorld();
	if (!World) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	// if (!OwnerPawn) return;
	// if (!OwnerPawn->IsLocallyControlled()) return;

	if (!StructureDataTable) return;
	
	const FName RowName = FName(*FString::FromInt(StructureIDDebug));
	static const FString ContextString(TEXT("BuildingSystem"));

	const auto Row = StructureDataTable->FindRow<FBuildTableInfo>(
		RowName,
		ContextString,
		true
		);

	if (Row)
	{
		BuildingClass = Row->BuildClass;
	}

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

void UBuildingComponent::SpawnBuildable(const FTransform& Transform, int32 BuildTemp)
{
	UWorld* World = GetWorld();
	if (!World) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = OwnerPawn;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FName RowName = FName(*FString::FromInt(BuildTemp));
	static const FString ContextString(TEXT("BuildingSystem"));

	const auto Row = StructureDataTable->FindRow<FBuildTableInfo>(
		RowName,
		ContextString,
		true
		);

	if (Row)
	{
		BuildingClass = Row->BuildClass;
	}
	
	ABuildableMaster* Preview = World->SpawnActor<ABuildableMaster>(BuildingClass, Transform, Params);
	if (!Preview) return;

	Preview->SetReplicates(true);
	Preview->SetReplicateMovement(false);

	Preview->SetActorEnableCollision(true);
}

void UBuildingComponent::SpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation, int32 StructureIDTemp)
{
	StructureIDDebug = StructureIDTemp;
	BuildTransform = Transform;

	if (BuildPlacementCheck(0, ClientCameraVector, ClientCameraRotation))
	{
		SpawnBuildable(Transform, StructureIDTemp);
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
	UWorld* World = GetWorld();
    if (!World) return false;

    if (!GetOwner() || !GetOwner()->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass())) return false;

    ASurvivalCharacter* SurvivalCharacter = Cast<ASurvivalCharacter>(ISurvivalCharacterInterface::Execute_GetSurvivalCharRef(GetOwner()));
    if (!SurvivalCharacter) return false;

    UCameraComponent* FirstPersonCamera = SurvivalCharacter->GetFirstPersonCamera();
    if (!FirstPersonCamera) return false;

    FVector CameraLocation = FirstPersonCamera->GetComponentLocation();

	// compute trace
	constexpr float BuildDistanceClose = 350.f;
	constexpr float BuildDistanceFar = 1000.f;
	
    FVector Forward = ClientCameraVector.GetSafeNormal();
    if (Forward.IsNearlyZero()) return false;

    FVector StartLocation = CameraLocation + Forward * BuildDistanceClose;
    FVector EndLocation = CameraLocation + Forward * BuildDistanceFar;

	// 确保BuildPreview存在，以保证可以获取到碰撞通道
    if (!IsValid(BuildPreview))
    {
        SpawnBuildPreview(StructureID);
        if (!IsValid(BuildPreview)) return false;
        BuildPreview->SetActorEnableCollision(false);
    }

	// 碰撞通道
	ECollisionChannel TraceChannel = BuildPreview->GetBuildableInfo().TraceChannel;
	
    FHitResult OutHit;
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(BuildPlacementTrace), true);
    TraceParams.AddIgnoredActor(GetOwner());
	if (IsValid(BuildPreview)) TraceParams.AddIgnoredActor(BuildPreview);

    const bool bHit = World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, TraceChannel, TraceParams);
	if (!bHit) return false;
	
	HitActor = OutHit.GetActor();
	HitComponent = OutHit.GetComponent();

    FVector DesiredLocation = bHit ? OutHit.ImpactPoint : EndLocation;
    FRotator DesiredRotation(0.f, ClientCameraRotation.Yaw + 90.f, 0.f);
    BuildTransform = FTransform(DesiredRotation, DesiredLocation);

	// 附着
	std::pair<bool, FTransform> Result = GetSnappingPoints();
    if (Result.first)
    {
        BuildTransform = Result.second;
        if (BuildPreview->GetBuildableInfo().DoFloatCheck)
        {
            return !CheckForOverlap() && IsBuildFloating();
        }
    	return !CheckForOverlap();
    }

	return !CheckForOverlap() && IsBuildFloating();
}

bool UBuildingComponent::IsBuildFloating() const
{
	UWorld* World = GetWorld();
	if (!World) return false;
	
	FHitResult OutHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(BuildPreview);
	
	bool bHit = World->LineTraceSingleByChannel(
		OutHit,
		BuildTransform.GetLocation(),
		BuildTransform.GetLocation() - FVector(0, 0, 50),
		ECC_Visibility,
		TraceParams
		);
	return bHit;
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

void UBuildingComponent::ChangeBuildStructure()
{
	if (IsValid(BuildPreview)) BuildPreview->Destroy();
	SpawnBuildPreview(StructureIDDebug);
}

void UBuildingComponent::ServerSpawnBuild_Implementation(const FTransform& Transform, const FVector& ClientCameraVector,
                                                         const FRotator& ClientCameraRotation, int32 StructureIDTemp)
{
	SpawnBuild(Transform, ClientCameraVector, ClientCameraRotation, StructureIDTemp);
}
