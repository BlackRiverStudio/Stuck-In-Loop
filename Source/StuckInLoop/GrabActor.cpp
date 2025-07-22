// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabActor.h"
#include "StuckInLoopCharacter.h"
#include <PhysicsEngine/PhysicsHandleComponent.h>

// Sets default values
AGrabActor::AGrabActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);

	GrabLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Grab Location"));
	GrabLocation->SetupAttachment(Mesh);
}

void AGrabActor::StartInteraction_Implementation(AActor* Interactor)
{
	IInteractionInterface::StartInteraction_Implementation(Interactor);

	if (const AStuckInLoopCharacter* Chara = Cast<AStuckInLoopCharacter>(Interactor))
	{
		if (UPhysicsHandleComponent* Handle = Chara->GetPhysicsHandle())
		{
			Handle->GrabComponentAtLocationWithRotation(
				Mesh,
				FName(),
				GrabLocation->GetComponentLocation(),
				GrabLocation->GetComponentRotation()
			);
		}
	}
}

void AGrabActor::EndInteraction_Implementation(AActor* Interactor)
{
	IInteractionInterface::EndInteraction_Implementation(Interactor);

	if (AStuckInLoopCharacter* Chara = Cast<AStuckInLoopCharacter>(Interactor))
	{
		if (UPhysicsHandleComponent* Handle = Chara->GetPhysicsHandle())
		{
			Handle->ReleaseComponent();
		}
	}
}