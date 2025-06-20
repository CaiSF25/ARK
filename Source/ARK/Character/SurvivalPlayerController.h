// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/HUD/MainWidget.h"
#include "ARK/Inventory/ItemInfo.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API ASurvivalPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(Client, Reliable)
	void UpdateItemSlot(const EContainerType& Container, int32 Index, const FItemInfo& ItemInfo);
	
	UFUNCTION(Client, Reliable)
	void ResetItemSlot(const EContainerType& Container, int32 Index);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMainWidget> WidgetClass;

	// 按键触发时调用
	UFUNCTION(BlueprintCallable, Category="UI")
	void HandleToggleInventory();

	// Client RPC: 切换UI
	UFUNCTION(Client, Reliable)
	void ToggleInventory(bool bShow);

	// Server RPC：客户端请求服务端翻转状态
	UFUNCTION(Server, Reliable)
	void ServerToggleInventory();
	
private:
	UPROPERTY()
	UMainWidget* MainWidget;

	bool bIsInventoryVisible = false;

public:
	
};
