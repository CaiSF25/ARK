// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInfo.h"
#include "Components/ActorComponent.h"
#include "ItemContainer.generated.h"


UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), BlueprintType)
class ARK_API UItemContainer : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemContainer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	
public:
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(const FItemInfo& Item);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory")
	void ServerAddItem(const FItemInfo& Item);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual bool AddItemToIndex(FItemInfo Item, int32 LocalSpecificIndex, int32 FromIndex);
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(int SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SwapItems(int32 SlotIndexA, int32 SlotIndexB);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool MergeStacks(int32 FromSlotIndex, int32 ToSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SplitStack(int32 SlotIndex, int32 SplitNum) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 FindEmptySlot() const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UpdateUI(int32 Index, const FItemInfo& Item, bool ResetSlot);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Inventory")
	void ServerOnSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex);

	void TransferItem(UItemContainer* ToComponent, const int32 ToSpecificIndex, const int32 ItemIndexToTransfer) const;

	EContainerType GetContainerType() const { return ContainerType; }

	UFUNCTION(BlueprintPure)
	FItemInfo GetItemAtIndex(const int32 Index) const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FItemInfo> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	EContainerType ContainerType;

	// 在子类中重写
	UFUNCTION(BlueprintCallable, Category="Inventory")
	virtual void HandleSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex);

private:
	UFUNCTION(BlueprintPure)
	bool IsSlotEmpty(int32 SlotIndex) const;

};
