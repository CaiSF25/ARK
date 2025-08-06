// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingSlot.h"

#include "ARK/Character/SurvivalPlayerController.h"
#include "ARK/Interfaces/GroundItemInterface.h"
#include "Components/Button.h"

void UCraftingSlot::OnSelectItemButtonClicked()
{
	const ASurvivalPlayerController* Controller = Cast<ASurvivalPlayerController>(GetOwningPlayer());

	Controller->GetMainWidget()->GetInventoryWidget()->GetCraftingWindow()->ShowItemRequirements(ItemID);
}

void UCraftingSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectItemButton)
	{
		auto MakeBrush = [&](UTexture2D* Texture, const FLinearColor& Color)->FSlateBrush
		{
			FSlateBrush Brush;
			if (Texture)
			{
				Brush.SetResourceObject(Texture);
				Brush.TintColor = Color;
			}
			return Brush;
		};

		FButtonStyle ButtonStyle;

		// Normal
		ButtonStyle.SetNormal(MakeBrush(ImageTexture, FLinearColor::White));

		// Hovered
		ButtonStyle.SetHovered(MakeBrush(ImageTexture, FLinearColor::Yellow));

		// Pressed
		ButtonStyle.SetPressed(MakeBrush(ImageTexture, FLinearColor::Green));

		// Disabled（如果需要也可填）
		// ButtonStyle.SetDisabled( … );
		// ButtonStyle.SetDisabledForeground( … );

		// —— 4. 应用到按钮 —— 
		SelectItemButton->SetStyle(ButtonStyle);

		SelectItemButton->OnClicked.AddDynamic(this, &UCraftingSlot::OnSelectItemButtonClicked);
	}
}
