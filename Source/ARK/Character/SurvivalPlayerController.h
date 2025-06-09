// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/HUD/MainWidget.h"
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
	UMainWidget* CreatedWidget;

	bool bIsInventoryVisible = false;
};
