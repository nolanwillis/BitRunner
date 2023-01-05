// Fill out your copyright notice in the Description page of Project Settings.

#include "Bit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

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
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 100.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 0.25f;
	
	// Set default player controller to posses pawn
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABit::BeginPlay()
{
	Super::BeginPlay();
	
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
	
	// Bind action mappings
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABit::ToggleJumpOn);
	InputComponent->BindAction("Jump", IE_Released, this, &ABit::ToggleJumpOff);
	InputComponent->BindAction("Ability1", IE_Pressed, this, &ABit::ToggleAbility1On);
	InputComponent->BindAction("Ability1", IE_Released, this, &ABit::ToggleAbility1Off);
	InputComponent->BindAction("Ability2", IE_Pressed, this, &ABit::ToggleAbility2On);
	InputComponent->BindAction("Ability2", IE_Released, this, &ABit::ToggleAbility2Off);

	// Bind axis mappings
	InputComponent->BindAxis("MoveLateral", this, &ABit::MoveY);
}

// Movement
void ABit::MoveY(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

// Jumping
void ABit::ToggleJumpOn()
{
	if (bCanJump)
	{
		// Set jump variables
		bJump = true;
		bCanJump = false;
		JumpTimeline.PlayFromStart();
	}
}
void ABit::ToggleJumpOff()
{
	bJump = false;
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
void ABit::ToggleAbility1On()
{
	bAbility1Triggered = true;
}
void ABit::ToggleAbility1Off()
{
	bAbility1Triggered = false;
}
void ABit::ToggleAbility2On()
{
	bAbility2Triggered = true;
}
void ABit::ToggleAbility2Off()
{
	bAbility2Triggered = false;
}


