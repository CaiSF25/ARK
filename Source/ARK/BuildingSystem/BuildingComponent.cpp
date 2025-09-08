
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
	if (!SurvivalCamera) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsValid(BuildPreview))
	{
		SpawnBuildPreview(StructureID);
	}

	constexpr int32 BuildDistanceClose = 350;
	constexpr int32 BuildDistanceFar = 1000;

	FVector Forward = SurvivalCamera->GetForwardVector();
	FVector Location = SurvivalCamera->GetComponentLocation();
	FRotator Rotation = SurvivalCamera->GetComponentRotation();
	
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
		SetPreviewColor(!CheckForOverlap() && bHit);
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

    const float BuildDistanceClose = 350.f;
    const float BuildDistanceFar = 1000.f;
	
    FVector Forward = ClientCameraVector.GetSafeNormal();
    if (Forward.IsNearlyZero()) return false;

    FVector StartLocation = CameraLocation + Forward * BuildDistanceClose;
    FVector EndLocation = CameraLocation + Forward * BuildDistanceFar;
	
    if (!IsValid(BuildPreview))
    {
        SpawnBuildPreview(StructureID);
        if (!IsValid(BuildPreview)) return false;
    	
        BuildPreview->SetActorEnableCollision(false);
    }
	
	ECollisionChannel TraceChannel = BuildPreview->GetBuildableInfo().TraceChannel;
	
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
