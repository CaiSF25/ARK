// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableTree.h"
#include "Components/CapsuleComponent.h"

ADestructableTree::ADestructableTree()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->SetupAttachment(SceneRoot);
	CapsuleComponent->SetRelativeLocation(FVector(0.f, 0.f, 402.f));
	const float DefaultHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight();
	CapsuleComponent->SetCapsuleHalfHeight(DefaultHalfHeight * 9);
	CapsuleComponent->SetSimulatePhysics(true);
	if (CapsuleComponent)
	{
		// 设置质量为200kg
		CapsuleComponent->SetMassOverrideInKg(NAME_None, 2000.0f, true);
	}
	CapsuleComponent->SetCollisionProfileName(TEXT("DestructibleTree"));
	
	CapsuleComponent1 = CreateDefaultSubobject<UCapsuleComponent>("Capsule1");
	CapsuleComponent1->SetupAttachment(CapsuleComponent);
	CapsuleComponent1->SetRelativeRotation(FRotator(0.f, 10.f, -75.f));
	CapsuleComponent1->SetRelativeLocation(FVector(0.f, 72.f, 370.f));
	CapsuleComponent1->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	if (CapsuleComponent1)
	{
		CapsuleComponent1->SetMassOverrideInKg(NAME_None, 400.0f, true);
	}
	CapsuleComponent1->SetCollisionProfileName(TEXT("DestructibleTree"));

	CapsuleComponent2 = CreateDefaultSubobject<UCapsuleComponent>("Capsule2");
	CapsuleComponent2->SetupAttachment(CapsuleComponent);
	CapsuleComponent2->SetRelativeRotation(FRotator(0.f, 10.f, 75.f));
	CapsuleComponent2->SetRelativeLocation(FVector(10.f, -72.f, 370.f));
	CapsuleComponent2->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	if (CapsuleComponent2)
	{
		CapsuleComponent2->SetMassOverrideInKg(NAME_None, 400.0f, true);
	}
	CapsuleComponent2->SetCollisionProfileName(TEXT("DestructibleTree"));

	CapsuleComponent3 = CreateDefaultSubobject<UCapsuleComponent>("Capsule3");
	CapsuleComponent3->SetupAttachment(CapsuleComponent);
	CapsuleComponent3->SetRelativeRotation(FRotator(75.f, 0.f, 0.f));
	CapsuleComponent3->SetRelativeLocation(FVector(72.f, 0.f, 370.f));
	CapsuleComponent3->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	if (CapsuleComponent3)
	{
		CapsuleComponent3->SetMassOverrideInKg(NAME_None, 400.0f, true);
	}
	CapsuleComponent3->SetCollisionProfileName(TEXT("DestructibleTree"));

	CapsuleComponent4 = CreateDefaultSubobject<UCapsuleComponent>("Capsule4");
	CapsuleComponent4->SetupAttachment(CapsuleComponent);
	CapsuleComponent4->SetRelativeRotation(FRotator(-75.f, 0.f, 0.f));
	CapsuleComponent4->SetRelativeLocation(FVector(-72.f, 0.f, 370.f));
	CapsuleComponent4->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	if (CapsuleComponent4)
	{
		CapsuleComponent4->SetMassOverrideInKg(NAME_None, 400.0f, true);
	}
	CapsuleComponent4->SetCollisionProfileName(TEXT("DestructibleTree"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(CapsuleComponent);
	StaticMesh->SetRelativeLocation(FVector(0.f, 0.f, -390.f));
	StaticMesh->SetCollisionProfileName(TEXT("DestructibleTree"));
}

void ADestructableTree::BeginPlay()
{
	Super::BeginPlay();
	
	AddForce();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DelayHandle,                       // 记录这个定时器
			this,                                  // 回调目标对象
			&ADestructableTree::OnDelayFinished,   // 回调函数
			10.f,                                  // Delay 时长（秒）
			false                                  // bLoop = false（只调用一次）
		);
	}
}

void ADestructableTree::AddForce() const
{
	constexpr  float Force = 1500;
	CapsuleComponent->AddForce(Force * Direction, NAME_None, true);
}

void ADestructableTree::OnDelayFinished()
{
	Destroy();
}
