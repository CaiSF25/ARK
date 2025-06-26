// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHotBar.h"

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


