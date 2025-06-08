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
	bool RemoveItem(int SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SwapItems(int32 SlotIndexA, int32 SlotIndexB);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool MergeStacks(int32 FromSlotIndex, int32 ToSlotIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SplitStack(int32 SlotIndex, int32 SplitNum) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 FindEmptySlot() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FItemInfo> Items;

};
