// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Buildscomp.generated.h"


class ABuildableMaster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARK_API UBuildscomp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildscomp();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:
	// 预览模式
	UFUNCTION(Client, Reliable)
	void ClientLaunchBuildMode(int32 StructureID);

	// 建造模式
	void OnSpawnBuild(const FTransform& Transform, const FVector& ClientCameraVector, const FRotator& ClientCameraRotation);

private:
	// 预览模式
	UPROPERTY(EditDefaultsOnly, Category = "Building")
	TSubclassOf<ABuildableMaster> BuildingClass;

	UPROPERTY()
	ABuildableMaster* BuildPreview;

	UPROPERTY(EditDefaultsOnly, Category = "Building|Preview")
	UMaterialInterface* GreenMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Building|Preview")
	UMaterialInterface* RedMaterial;
	


	

		
};
