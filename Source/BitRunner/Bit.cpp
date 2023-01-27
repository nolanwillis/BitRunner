// Fill out your copyright notice in the Description page of Project Settings.

#include "Bit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
	// Setup jump curves
	if (JumpHeightCurve)
	{                           
		// Bind jump functions to respective delegates
		FOnTimelineFloat JumpHeightProgressUpdate;
		JumpHeightProgressUpdate.BindUFunction(this, FName("JumpHeightUpdate"));
		FOnTimelineEvent JumpHeightFinishedEvent;
		JumpHeightFinishedEvent.BindUFunction(this, FName("JumpHeightFinished"));

		// Add interp float curve and to jump timeline and register
		// update/finish functions
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
		JumpEndRot.Roll = 359.9999;
		JumpEndRot.Pitch = 359.9999;
	}
}

// Called every frame
void ABit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles y direction movement
	{
		if (MovementInput)
		{
			MovementInput = MovementInput * MovementSpeedY;
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorRightVector() * MovementInput * DeltaTime;
			SetActorLocation(NewLocation);
			MovementInput = 0;
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
	// If timelines are played tick it using delta time
	JumpTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ABit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// Bind input actions
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABit::Jump);
	EnhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Triggered, this, &ABit::TriggerAbility1);
	EnhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Triggered, this, &ABit::TriggerAbility2);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABit::Move);
}

// Movement
void ABit::Move(const FInputActionValue& Value)
{
	MovementInput = Value.Get<float>();
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
		MovementSpeedX += 100;
		MovementSpeedY += 100;
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
	MovementSpeedX -= 100;
	MovementSpeedY -= 100;
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
	if (Value.Get<bool>() && bAbility1_CanTrigger)
	{
		bAbility1_Triggered = true;
		bAbility1_CanTrigger = false;
		Ability1_Charge = 100.0f;
		FVector Start = GetActorLocation() + FVector(0, 0, 30);
		FVector End = Start + FVector(Ability1_Range*-1, 0, 0);
		GetWorld()->LineTraceMultiByChannel(Ability1_Hits, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
		UE_LOG(LogTemp, Warning, TEXT("%d items were hit"), Ability1_Hits.Num());
		for (FHitResult Item : Ability1_Hits)
		{
			Item.GetActor()->Destroy();
		}
		GetWorldTimerManager().SetTimer(Ability1_TimerHandle, this, &ABit::TickAbility1Timer, 1.0f, true);
		DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), true, -1, 0, 25);
	}
}
void ABit::TriggerAbility2(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && bAbility2_CanTrigger)
	{
		bAbility2_Triggered = true;
		bAbility2_CanTrigger = false;
		Ability2_Charge = 100.0f;
		GetWorldTimerManager().SetTimer(Ability2_TimerHandle, this, &ABit::TickAbility2Timer, 1.0f, true);
	}
}

void ABit::TickAbility1Timer()
{
	Ability1_Charge -= Ability1_ChargeRate;
	if (Ability1_Charge <= 0)
	{
		GetWorldTimerManager().ClearTimer(Ability1_TimerHandle);
		bAbility1_CanTrigger = true;
	}
}

void ABit::TickAbility2Timer()
{
	Ability2_Charge -= Ability2_ChargeRate;
	if (Ability2_Charge <= 0)
	{
		GetWorldTimerManager().ClearTimer(Ability2_TimerHandle);
		bAbility2_CanTrigger = true;
	}
}




