// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STUCKINLOOP_API IInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Jam|Interaction")
	void StartInteraction(AActor* Interactor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Jam|Interaction")
	void EndInteraction(AActor* Interactor);
};
