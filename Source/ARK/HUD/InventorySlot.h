// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DraggedItem.h"
#include "ARK/Structures/ItemInfo.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	EContainerType ContainerType;
	
	UFUNCTION(BlueprintCallable)
	void UpdateSlot(const FItemInfo& LocalItemInfo);

	UFUNCTION(BlueprintCallable)
	void ResetSlot();

	UFUNCTION(BlueprintCallable)
	void SetItemIndex(const int32 LocalItemIndex){ ItemIndex = LocalItemIndex; }

	UFUNCTION(BlueprintCallable)
	void SetContainerType(const EContainerType LocalContainerType) { ContainerType = LocalContainerType; }

	UFUNCTION(BlueprintCallable)
	void SetBackgroundImage(UTexture2D* InBackgroundImage);

	// 护甲
	UFUNCTION(BlueprintCallable)
	void SetArmorType(const EArmorType& InArmorType);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UBorder* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TopText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ItemHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* QuantityText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UBorder* SlotStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn))
	UTexture2D* BackgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn))
	bool bArmorSlot = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn))
	EArmorType ArmorType;
	
private:
	bool HasItemInSlot = false;

	FItemInfo ItemInfo;

	int32 ItemIndex;

public:
	UPROPERTY(EditAnywhere, Category="Drag")
	TSubclassOf<UDraggedItem> DraggedItem;
};
