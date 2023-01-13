// Fill out your copyright notice in the Description page of Project Settings.

#include "Bit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ABit::ABit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	// Attach components
	StaticMesh->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	// Set values for spring arm
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-15.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 150.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 1.0f;
	
	// Set default player controller to posses pawn
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABit::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!InputMapping)
			{
				InputSystem->AddMappingContext(InputMapping, 0);
			}
		}
	}
	
	// Setup jump curves
	if (JumpHeightCurve)
	{
		// Bind jump functions to respective delegates
		FOnTimelineFloat JumpHeightProgressUpdate;
		JumpHeightProgressUpdate.BindUFunction(this, FName("JumpHeightUpdate"));
		FOnTimelineEvent JumpHeightFinishedEvent;
		JumpHeightFinishedEvent.BindUFunction(this, FName("JumpHeightFinished"));

		// Add interp float and finish functions to bit timeline
		JumpTimeline.AddInterpFloat(JumpHeightCurve, JumpHeightProgressUpdate);
		JumpTimeline.SetTimelineFinishedFunc(JumpHeightFinishedEvent);

		// Set start and end jump targets
		JumpStartLoc = JumpEndLoc = GetActorLocation();
		JumpEndLoc.Z += JumpHeight;
	}
	if (JumpTwistCurve) {
		// Bind twist functions to respective delegates
		FOnTimelineFloat JumpTwistProgressUpdate;
		JumpTwistProgressUpdate.BindUFunction(this, FName("JumpTwistUpdate"));

		// Add interp float to bit timeline
		JumpTimeline.AddInterpFloat(JumpTwistCurve, JumpTwistProgressUpdate);
		
		// Set start rotation of the bit 
		JumpStartRot = JumpEndRot = StaticMesh->GetRelativeRotation();
		JumpEndRot.Yaw = 359.9999;
	}
}

// Called every frame
void ABit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles y direction movement
	{
		if (!MovementInput.IsZero())
		{
			MovementInput = MovementInput.GetSafeNormal() * MovementSpeedY;
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
			SetActorLocation(NewLocation);
			// Update y component of the jump start/end location vectors 
			JumpStartLoc.Y = GetActorLocation().Y;
			JumpEndLoc.Y = GetActorLocation().Y;
		}
	}
	// Handles x direction movement (this movement is automatic, no input)
	{
		if (bXMovementEnabled)
		{
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorForwardVector() * MovementSpeedX * DeltaTime;
			SetActorLocation(NewLocation);
			// Update x component of the jump start/end location vectors
			JumpStartLoc.X = GetActorLocation().X;
			JumpEndLoc.X = GetActorLocation().X;
		}
	}
	// If timeline is played tick it using delta time
	JumpTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ABit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (EnhancedInputComponent)
	{
		// Bind input actions
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABit::Jump);
		EnhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Triggered, this, &ABit::TriggerAbility1);
		EnhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Triggered, this, &ABit::TriggerAbility2);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABit::Move);
	}
}

// Movement
void ABit::Move(const FInputActionValue& Value)
{
	MovementInput.Y = FMath::Clamp<float>(Value.Get<float>(), -1.0f, 1.0f);
}

// Jumping
void ABit::Jump(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && bCanJump)
	{
		// Set jump variables
		bJump = true;
		bCanJump = false;
		JumpTimeline.PlayFromStart();
	}
}
void ABit::JumpHeightUpdate(float Value) 
{
	FVector NewLocation = FMath::Lerp(JumpStartLoc, JumpEndLoc, Value);
	SetActorLocation(NewLocation);
}
void ABit::JumpHeightFinished()
{
	bCanJump = true;
}
void ABit::JumpTwistUpdate(float Value)
{
	FRotator NewRotation = FRotator(FQuat::SlerpFullPath(
		JumpStartRot.Quaternion(), JumpEndRot.Quaternion(), Value));
	StaticMesh->SetRelativeRotation(NewRotation);
}

// Abilities
void ABit::TriggerAbility1(const FInputActionValue& Value)
{
	bAbility1Triggered = Value.Get<bool>();
}
void ABit::TriggerAbility2(const FInputActionValue& Value)
{
	bAbility2Triggered = Value.Get<bool>();
}



