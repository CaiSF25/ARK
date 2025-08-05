// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingSlot.generated.h"

class UButton;
enum class EContainerType : uint8;
/**
 * 
 */
UCLASS()
class ARK_API UCraftingSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetImageTexture(UTexture2D* Texture) {ImageTexture = Texture;}

	void SetItemID(const int32 LocalItemID) {ItemID = LocalItemID;}

	void SetContainerType(const EContainerType LocalContainerType) {ContainerType = LocalContainerType;}

	void SetItemIndex(const int32 LocalItemIndex) {ItemIndex = LocalItemIndex;}

protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingSlot", meta = (ExposeOnSpawn))
	UTexture2D* ImageTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingSlot", meta = (ExposeOnSpawn))
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingSlot", meta = (ExposeOnSpawn))
	EContainerType ContainerType;

	UPROPERTY()
	bool CanCraftItem;

	UPROPERTY()
	int32 ItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	UTexture2D* NormalTex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	UTexture2D* HoveredTex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	UTexture2D* PressedTex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* SelectItemButton;
};
