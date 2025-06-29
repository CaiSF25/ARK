// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DraggedItem.h"
#include "ARK/Inventory/ItemInfo.h"
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
	UFUNCTION(BlueprintCallable)
	void UpdateSlot(const FItemInfo& LocalItemInfo);

	UFUNCTION(BlueprintCallable)
	void ResetSlot();

	UFUNCTION(BlueprintCallable)
	void SetItemIndex(const int32 LocalItemIndex){ ItemIndex = LocalItemIndex; }

	UFUNCTION(BlueprintCallable)
	void SetContainerType(EContainerType LocalContainerType) { ContainerType = LocalContainerType; }
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
	
private:
	bool HasItemInSlot = false;

	FItemInfo ItemInfo;

	int32 ItemIndex;

	EContainerType ContainerType;

public:
	UPROPERTY(EditAnywhere, Category="Drag")
	TSubclassOf<UDraggedItem> DraggedItem;
};
