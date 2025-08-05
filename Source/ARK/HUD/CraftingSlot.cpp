// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingSlot.h"

#include "Components/Button.h"

void UCraftingSlot::NativeConstruct()
{
	Super::NativeConstruct();

	auto MakeBrush = [&](UTexture2D* Texture)->FSlateBrush
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		return Brush;
	};

	auto MakeColor = [&](const FLinearColor& C)->FSlateColor
	{
		return FSlateColor(C);
	};

	FButtonStyle ButtonStyle;

	// Normal
	ButtonStyle.SetNormal   ( MakeBrush(NormalTex) );
	ButtonStyle.SetNormalForeground(MakeColor(FLinearColor::Yellow));

	// Hovered
	ButtonStyle.SetHovered  ( MakeBrush(HoveredTex) );
	ButtonStyle.SetHoveredForeground(MakeColor(FLinearColor::Green));

	// Pressed
	ButtonStyle.SetPressed  ( MakeBrush(PressedTex) );
	ButtonStyle.SetPressedForeground(MakeColor(FLinearColor::Red));

	// Disabled（如果需要也可填）
	// ButtonStyle.SetDisabled( … );
	// ButtonStyle.SetDisabledForeground( … );

	// —— 4. 应用到按钮 —— 
	SelectItemButton->SetStyle(ButtonStyle);
}
