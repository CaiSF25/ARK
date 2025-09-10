// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildableMaster.h"
#include "ARK/Interfaces/InteractableInterface.h"
#include "DoorMaster.generated.h"

class UTimelineComponent;
/**
 * 
 */
UCLASS()
class ARK_API ADoorMaster : public ABuildableMaster, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ADoorMaster();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsDoorOpen)
	bool bIsDoorOpen = false;

	UFUNCTION()
	void OnRep_IsDoorOpen();
	
	// Timeline 组件
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DoorTimeline;

	// 曲线资源（在编辑器赋值）
	UPROPERTY(EditAnywhere, Category="DoorTimeline")
	UCurveFloat* DoorCurve;

	// Timeline 绑定的回调：进度值（0..1）
	UFUNCTION()
	void TimelineProgress(float Value);

	// 门初始属性和开关门设置
	float InitialYaw = 0.f;
	bool bHasInitialYaw = false;

	UPROPERTY(EditAnywhere, Category="Door")
	float DoorOpenAngle = -90.f;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetDoorRotation(const bool OpenOrClose);

public:
	// 接口实现
	virtual void InteractEvent_Implementation(ASurvivalCharacter* CharRef) override;
};
