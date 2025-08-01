// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableRock.h"

#include "DestructionForce.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Kismet/GameplayStatics.h"

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

			MulticastFX();

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

void ADestructableRock::MulticastFX_Implementation()
{
	if (!DestructionParticle)
	{
		UE_LOG(LogTemp, Warning, TEXT("未设置粒子系统资源"));
		return;
	}
	
	const UWorld* World = GetWorld();
	if (!World) return;

	const FVector SpawnLocation = GetActorLocation();
	const FRotator DefaultRotation = FRotator::ZeroRotator;
	const FVector DefaultScale = FVector(1.0f);
	UGameplayStatics::SpawnEmitterAtLocation(
		World,
		DestructionParticle,
		SpawnLocation,
		DefaultRotation,
		DefaultScale
	);

	UGameplayStatics::PlaySoundAtLocation(
		World,
		DestructionSound,
		SpawnLocation,
		DefaultRotation,
		1.0,
		1.0,
		0.0,
		DestructionAttenuation
	);
}
