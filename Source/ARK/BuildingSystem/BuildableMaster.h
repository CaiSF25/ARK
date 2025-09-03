// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/Interfaces/BuildInterface.h"
#include "ARK/Structures/BuildableInfo.h"
#include "GameFramework/Actor.h"
#include "BuildableMaster.generated.h"

UCLASS()
class ARK_API ABuildableMaster : public AActor, public IBuildInterface
{
	GENERATED_BODY()
	
public:	
	ABuildableMaster();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// 接口实现
	virtual ABuildableMaster* GetBuildMasterRef_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FBuildableInfo BuildableInfo;

private:
};
