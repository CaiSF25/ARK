// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHotBar.h"

#include "PlayerInventory.h"
#include "ARK/Character/SurvivalCharacter.h"
#include "ARK/Character/SurvivalPlayerController.h"

UPlayerHotBar::UPlayerHotBar()
{
	Items.SetNum(10);
}

EItemType UPlayerHotBar::CheckHotbar(int32 Index)
{
	if (Items[Index].ItemID == 0)
	{
		return EItemType::None;
	}
	return Items[Index].ItemType;
}

bool UPlayerHotBar::AddItemToIndex(FItemInfo Item, int32 LocalSpecificIndex, int32 FromIndex)
{
	bool bSuccess = Super::AddItemToIndex(Item, LocalSpecificIndex, FromIndex);
	AActor* Owner = GetOwner();
	if (!Owner) return false;
	ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Owner);
	if (!Character) return false;
	if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		const ISurvivalCharacterInterface* Interface = Cast<ISurvivalCharacterInterface>(Character);
		ASurvivalPlayerController* PlayerController = Cast<ASurvivalPlayerController>(Interface->Execute_GetControllerFromChar(Character));
		if (!PlayerController) return false;
		PlayerController->UpdateItemSlot(ContainerType, LocalSpecificIndex, GetItemAtIndex(LocalSpecificIndex));
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(LocalSpecificIndex));
	return bSuccess;
}

bool UPlayerHotBar::RemoveItemAtIndex(const int32 Index)
{
	bool bSuccess = Super::RemoveItemAtIndex(Index);

	AActor* Owner = GetOwner();
	if (!Owner) return false;
	ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Owner);
	if (!Character) return false;
	if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		ISurvivalCharacterInterface* Interface = Cast<ISurvivalCharacterInterface>(Character);
		ASurvivalPlayerController* PlayerController = Cast<ASurvivalPlayerController>(Interface->Execute_GetControllerFromChar(Character));
		if (!PlayerController) return false;
		PlayerController->ResetItemSlot(ContainerType, Index);
		Character->OnDequipCurItem(Index);
	}
	
	return bSuccess;
}

void UPlayerHotBar::HandleSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex)
{
	FromContainer->TransferItem(this, DroppedIndex, FromIndex);
	Super::HandleSlotDrop(FromContainer, FromIndex, DroppedIndex);
}


