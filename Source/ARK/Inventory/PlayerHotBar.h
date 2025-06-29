// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "PlayerHotBar.generated.h"

/**
 * 
 */
UCLASS()
class ARK_API UPlayerHotBar : public UItemContainer
{
	GENERATED_BODY()

public:
	UPlayerHotBar();

public:
	UFUNCTION(BlueprintCallable)
	EItemType CheckHotbar(int32 Index);

	virtual bool AddItemToIndex(FItemInfo Item, int32 LocalSpecificIndex, int32 FromIndex) override;

	virtual bool RemoveItemAtIndex(const int32 Index) override;
	
protected:
	virtual void HandleSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex) override;
};
