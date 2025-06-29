// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventory.h"

#include "ARK/Character/SurvivalCharacter.h"

UPlayerInventory::UPlayerInventory()
{
	Items.SetNum(30);
}

bool UPlayerInventory::AddItemToIndex(FItemInfo Item, int32 LocalSpecificIndex, int32 FromIndex)
{
	bool bSuccess = Super::AddItemToIndex(Item, LocalSpecificIndex, FromIndex);
	AActor* Owner = GetOwner();
	if (!Owner) return false;
	ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Owner);
	if (!Character) return false;
	if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		ISurvivalCharacterInterface* Interface = Cast<ISurvivalCharacterInterface>(Character);
		ASurvivalPlayerController* PlayerController = Interface->Execute_GetControllerFromChar(Character);
		if (!PlayerController) return false;
		PlayerController->UpdateItemSlot(ContainerType, LocalSpecificIndex, GetItemAtIndex(LocalSpecificIndex));
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(LocalSpecificIndex));
	return bSuccess;
}

bool UPlayerInventory::RemoveItemAtIndex(const int32 Index)
{
	const bool bSuccess = Super::RemoveItemAtIndex(Index);

	AActor* Owner = GetOwner();
	if (!Owner) return false;
	ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Owner);
	if (!Character) return false;
	if (Character->GetClass()->ImplementsInterface(USurvivalCharacterInterface::StaticClass()))
	{
		ISurvivalCharacterInterface* Interface = Cast<ISurvivalCharacterInterface>(Character);
		ASurvivalPlayerController* PlayerController = Interface->Execute_GetControllerFromChar(Character);
		if (!PlayerController) return false;
		PlayerController->ResetItemSlot(ContainerType, Index);
	}
	
	return bSuccess;
}

void UPlayerInventory::HandleSlotDrop(UItemContainer* FromContainer, int32 FromIndex, int32 DroppedIndex)
{
	FromContainer->TransferItem(this, DroppedIndex, FromIndex);
	Super::HandleSlotDrop(FromContainer, FromIndex, DroppedIndex);
}
