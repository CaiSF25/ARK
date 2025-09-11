// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildableMaster.h"

#include "Components/BoxComponent.h"

ABuildableMaster::ABuildableMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// 根节点
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	// 模型
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Root);

	// 自定义碰撞
	OverlapBox = CreateDefaultSubobject<UBoxComponent>("OverlapBox");
	OverlapBox->SetupAttachment(Root);
	OverlapBox->SetBoxExtent(FVector(32, 32, 32));
	OverlapBox->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ABuildableMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABuildableMaster::BeginPlay()
{
	Super::BeginPlay();
	
}

ABuildableMaster* ABuildableMaster::GetBuildMasterRef_Implementation()
{
	return this;
}

