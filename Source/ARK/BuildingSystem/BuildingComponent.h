// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingComponent.generated.h"


class UDataTable;
class UCameraComponent;
class ABuildableMaster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARK_API UBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StructureIDDebug;
	
	bool GetIsBuildModeEnabled() const { return bIsBuildModeEnabled; }

	void SetIsBuildModeEnabled(const bool InValue) { bIsBuildModeEnabled = InValue; }

	FTransform GetBuildTransform() const { return BuildTransform; }

	// 预览模式
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientLaunchBuildMode(int32 StructureID);

	// 建造模式
	void OnSpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation);

private:
	// 组件
	UPROPERTY()
	UCameraComponent* SurvivalCamera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	UDataTable* StructureDataTable;
	
	// 预览模式
	bool bIsBuildModeEnabled = false;

	FTimerHandle BuildLoopTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	ABuildableMaster* BuildPreview;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	FTransform BuildTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABuildableMaster> BuildingClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* GreenMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* RedMaterial;
	
	UFUNCTION()
	void BuildMode(int32 StructureID);

	void SpawnBuildPreview(int32 StructureID);
	
	void StartBuildLoop(int32 StructureID);

	void StopBuildLoop();

	void SetPreviewColor(bool bCanPlace) const;

	// 建造模式
	void SpawnBuildable(const FTransform& Transform);
	
	void SpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation);

	UFUNCTION(Server, Reliable)
	void ServerSpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation);

	bool CheckForOverlap() const;

	bool BuildPlacementCheck(const int32 StructureID, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation);

	// 建筑吸附
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* HitComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingSystem", meta = (AllowPrivateAccess = "true"))
	AActor* HitActor;
	
	std::pair<bool, FTransform> GetSnappingPoints();
};
