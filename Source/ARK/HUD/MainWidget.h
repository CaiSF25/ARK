// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hotbar.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.h"
#include "PlayerStatsWindow.h"
#include "MainWidget.generated.h"

class ULevelNotification;
class UExperienceText;
class UStateNotifycationWidget;
class UCraftingProgressBar;
class UVerticalBox;
class UItemAdded;
class USizeBox;
/**
 * 
 */
UCLASS()
class ARK_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

	UFUNCTION()
	UHotbar* GetHotbar() const { return Hotbar; }
	
	UFUNCTION()
	void SetInventoryVisibility(const bool bVisible) const;

	UFUNCTION(BlueprintCallable)
	void ShowItemCollected(UTexture2D* ResourceImage, int32 ResourceQuantity, FText ResourceName);

	UFUNCTION()
	void ShowCraftingBar(const float InTime) const;

	// 玩家状态
	UFUNCTION()
	UPlayerStatsWindow* GetPlayerStatsWindow() const { return PlayerStatsWindow; }

	UFUNCTION()
	void ShowStarvingNotification();
	
	UFUNCTION()
	void ShowDehydratedNotification();

	UFUNCTION()
	void RemoveStarvingNotification() const;

	UFUNCTION()
	void RemoveDehydratedNotification() const;

	// 玩家经验与升级
	void AddExperienceNotification(int32 ExperienceAmount);

	void ShowLevelUpNotification(int32 NewLevel);

	void RemoveLevelUpNotification() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UItemAdded> ItemAddedClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCraftingProgressBar> CraftingProgressBarClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UStateNotifycationWidget> StateNotifycationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UExperienceText> ExperienceTextClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULevelNotification> LevelNotificationClass;
	
private:
	UPROPERTY(meta=(BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	UHotbar* Hotbar;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* ResourceCollected;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* CraftWindowBox;

	UPROPERTY(meta=(BindWidget))
	UPlayerStatsWindow* PlayerStatsWindow;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* NotificationBox;

	UPROPERTY()
	UStateNotifycationWidget* StarvingWidget;

	UPROPERTY()
	UStateNotifycationWidget* DehydratedWidget;

	UPROPERTY()
	UExperienceText* ExperienceText;

	UPROPERTY()
	ULevelNotification* LevelNotification;
};
