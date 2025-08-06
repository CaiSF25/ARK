// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingIngredient.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCraftingIngredient::NativeConstruct()
{
	Super::NativeConstruct();

	const FString ResultStr = FString::Printf(TEXT("x%d %s"),
		ResourceQuantity,
		*ResourceText.ToString()
	);

	const FText FinalText = FText::FromString(ResultStr);

	ResourceName->SetText(FinalText);

	ResourceImage->SetBrushFromTexture(ResourceIcon);
}
