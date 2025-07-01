// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableRock.h"

#include "DestructionForce.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionObject.h"

ADestructableRock::ADestructableRock()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	GeometryCollectionComponent->SetupAttachment(RootComponent);
	GeometryCollectionComponent->SetCollisionProfileName(TEXT("DestructibleItem"));
	GeometryCollectionComponent->DamageThreshold[0] = 5000.f;
	GeometryCollectionComponent->DamageThreshold[1] = 500.f;
	GeometryCollectionComponent->DamageThreshold[2] = 50.f;
}

void ADestructableRock::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->SpawnActor<ADestructionForce>(
				ADestructionForce::StaticClass(),
				GetActorLocation(),
				FRotator(0, 0, 0)
				);

			World->GetTimerManager().SetTimer(
				DelayHandle,                       // 记录这个定时器
				this,                                  // 回调目标对象
				&ADestructableRock::OnDelayFinished,   // 回调函数
				8.f,                                  // Delay 时长（秒）
				false                                  // bLoop = false（只调用一次）
				);
		}
	}
}

void ADestructableRock::OnDelayFinished()
{
	Destroy();
}
