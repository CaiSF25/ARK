// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARK/HUD/MainWidget.h"
#include "ARK/Interfaces/SurvivalCharacterInterface.h"
#include "ARK/Interfaces/PlayerControllerInterface.h"
#include "ARK/Inventory/ItemInfo.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API ASurvivalPlayerController : public APlayerController, public IPlayerControllerInterface
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;
	
protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(Client, Reliable)
	void UpdateItemSlot(const EContainerType& Container, int32 Index, const FItemInfo& ItemInfo);
	
	UFUNCTION(Client, Reliable)
	void ResetItemSlot(const EContainerType& Container, int32 Index);

	UFUNCTION(Client, Reliable)
	void UpdateCraftStatus(bool CanCraft);

	UFUNCTION(Client, Reliable)
	void ShowCraftingProgressBar(const float InTime);

	// 按键触发时调用
	UFUNCTION(BlueprintCallable, Category="UI")
	void HandleToggleInventory();

	UFUNCTION(BlueprintCallable, Category="UI")
	UInventorySlot* GetInventoryWidget(EContainerType Container, int32 SlotIndex);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ShowItemWidget(UTexture2D* ResourceImage, int32 ResourceQuantity, const FText& ResourceName);

	UFUNCTION()
	UMainWidget* GetMainWidget() const { return MainWidget; }

	// 接口实现
	virtual AController* SurvivalGamePCRef_Implementation() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMainWidget> WidgetClass;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* UIInputContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess = "true"))
	class UInputAction* OpenUIAction;
};
