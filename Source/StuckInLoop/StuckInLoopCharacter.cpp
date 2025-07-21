// Copyright Epic Games, Inc. All Rights Reserved.

#include "StuckInLoopCharacter.h"
#include "StuckInLoopProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include <PhysicsEngine/PhysicsHandleComponent.h>
#include "InteractionInterface.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AStuckInLoopCharacter

AStuckInLoopCharacter::AStuckInLoopCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	PhysicsHandleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Physics Handle Location"));
	PhysicsHandleLocation->SetupAttachment(GetCapsuleComponent());
	PhysicsHandleLocation->SetRelativeLocation({ 150.f, 0.f, 40.f });
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Physics Handle"));
}

void AStuckInLoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PhysicsHandle && PhysicsHandleLocation)
	{
		PhysicsHandle->SetTargetLocationAndRotation(
			PhysicsHandleLocation->GetComponentLocation(),
			PhysicsHandleLocation->GetComponentRotation()
		);
	}
}

void AStuckInLoopCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////// Input

void AStuckInLoopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AStuckInLoopCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AStuckInLoopCharacter::Look);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AStuckInLoopCharacter::Interact);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AStuckInLoopCharacter::GetInFrontOfChara(const TEnumAsByte<ECollisionChannel> TraceChannel, float Distance, TArray<class AActor*> ActorsToIgnore, FHitResult& Hit, bool& bDidHit)
{
	FTransform CharaTrans = FirstPersonCameraComponent->GetComponentTransform();
	const FVector CharaForward = UKismetMathLibrary::GetForwardVector(CharaTrans.Rotator());
	const FVector Start = CharaTrans.GetLocation() + CharaForward;
	const FVector End = Start + CharaForward * Distance;
	FBoxSphereBounds Bounds = FBoxSphereBounds();
	if (const USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		if (const USkeletalMesh* SkelMesh = MeshComponent->GetSkeletalMeshAsset())
		{
			Bounds = SkelMesh->GetBounds();
		}
	}
	const FCollisionShape Shape = FCollisionShape::MakeBox(Bounds.BoxExtent);
	FCollisionQueryParams Params;
	Params.AddIgnoredActors(ActorsToIgnore);
	bDidHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FRotator::ZeroRotator.Quaternion(), TraceChannel, Shape, Params);
}


void AStuckInLoopCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AStuckInLoopCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AStuckInLoopCharacter::Interact(const FInputActionValue& Value)
{
	if (LastInteracted.GetObject())
	{
		LastInteracted->Execute_EndInteraction(LastInteracted.GetObject(), this);

		LastInteracted.SetInterface(nullptr);
		LastInteracted.SetObject(nullptr);
	}
	else
	{
		bool bDidHit = false;
		FHitResult Hit;
		TArray<AActor*> Ignored;
		Ignored.Add(this);

		// add that fucking function
		GetInFrontOfChara(ECC_Visibility, InteractionDistance, Ignored, Hit, bDidHit);

		if (bDidHit)
		{
			if (Hit.GetActor()->Implements<IInteractionInterface>())
			{
				LastInteracted.SetInterface(Cast<IInteractionInterface>(Hit.GetActor()));
				LastInteracted.SetObject(Hit.GetActor());
				LastInteracted->Execute_StartInteraction(Hit.GetActor(), this);
			}
		}
	}
}