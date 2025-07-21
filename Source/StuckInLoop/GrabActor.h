// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "GrabActor.generated.h"

UCLASS()
class STUCKINLOOP_API AGrabActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabActor();

	virtual void StartInteraction_Implementation(AActor* Interactor);
	virtual void EndInteraction_Implementation(AActor* Interactor);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jam|Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jam|Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* GrabLocation;
};
