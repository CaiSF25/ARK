// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SurvivalCharacterInterface.generated.h"

class ASurvivalPlayerController;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USurvivalCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARK_API ISurvivalCharacterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	AController* GetControllerFromChar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	APawn* GetSurvivalCharRef();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void ThirdPersonMontage(UAnimMontage* ThirdPersonMontage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void FirstPersonMontage(UAnimMontage* FirstPersonMontage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	FVector GetArrowLocation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	FRotator GetArrowRotation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	int32 GetTotalArmorPieces();
};
