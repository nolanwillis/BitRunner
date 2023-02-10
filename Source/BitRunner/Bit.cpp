// Fill out your copyright notice in the Description page of Project Settings.

#include "Bit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Projectile.h"


// Sets default values
ABit::ABit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
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
		if (MovementInput && bYMovementEnabled)
		{
			MovementInput = MovementInput * MovementSpeedY;
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorRightVector() * MovementInput * DeltaTime;
			// Since player is always on track just limit movement input
			// to a specified range instead of using colliders.
			if (NewLocation.Y < MaxY && NewLocation.Y > MinY)
			{
				SetActorLocation(NewLocation);
			}
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
	// Checks if player is dead
	{
		if (bIsDead)
		{
			CurrentHealth = 0.0f;
			bXMovementEnabled = false;
			bYMovementEnabled = false;
			bAbility1_CanTrigger = false;
			bAbility2_CanTrigger = false;
			bCanJump = false;

			Camera->DetachFromParent(true, false);
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
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABit::Move);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABit::Jump);
	EnhancedInputComponent->BindAction(FireAbilityAction, ETriggerEvent::Triggered, this, &ABit::TriggerFireAbility);
	EnhancedInputComponent->BindAction(Ability1_Action, ETriggerEvent::Triggered, this, &ABit::TriggerAbility1);
	EnhancedInputComponent->BindAction(Ability2_Action, ETriggerEvent::Triggered, this, &ABit::TriggerAbility2);
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
void ABit::TriggerFireAbility(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && bFireAbility_CanTrigger)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fired!"));
		bFireAbility_Triggered = true;
		bFireAbility_CanTrigger = true;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation() + FVector(-100, 0, 50));
		FActorSpawnParameters Params;
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(FireProjectile, SpawnTransform, Params);
	}
}
void ABit::TriggerAbility1(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && bAbility1_CanTrigger)
	{
		bAbility1_Triggered = true;
		// Disable ability
		bAbility1_CanTrigger = false;
		// Set charge to empty (100 = no charge, 0 = charged, this is inverted due to UI)
		Ability1_Charge = 100.0f;
		// Get start and end location for line trace
		FVector Start = GetActorLocation() + FVector(0, 0, 50);
		FVector End = Start + FVector(Ability1_Range*-1, 0, 0);
		// Cast line trace, store hit results in Ability1_Hits array
		GetWorld()->LineTraceMultiByChannel(Ability1_Hits, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
		UE_LOG(LogTemp, Warning, TEXT("%d items were hit"), Ability1_Hits.Num());
		// Delete actor associated with each hit result
		for (FHitResult Item : Ability1_Hits)
		{
			Item.GetActor()->Destroy();
		}
		// Start ability1 cooldown timer
		GetWorldTimerManager().SetTimer(Ability1_CooldownTimer, this, &ABit::TickAbility1Cooldown, 1.0f, true);
		DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), true, -1, 0, 25);
	}
}
void ABit::TriggerAbility2(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && bAbility2_CanTrigger)
	{
		bAbility2_Triggered = true;
		// Make player immune to damage
		bImmune = true;
		// Disable ability
		bAbility2_CanTrigger = false;
		// Set charge to empty
		Ability2_Charge = 100.0f;
		// Start ability2 duration timer (when ability2 fires for a given duration, it will then start cooldown timer)
		GetWorldTimerManager().SetTimer(Ability2_DurationTimer, this, &ABit::TickAbility2Duration, 1.0f, true);
	}
}
// Functions called during timer ticks 
void ABit::TickAbility1Cooldown()
{
	// Every tick decrement charge (this charges the ability since charge value is inverted)
	Ability1_Charge -= Ability1_ChargeRate;
	// If ability1 is charged (Ability1_Charge == 0) clear cooldown timer and enable ability
	if (Ability1_Charge == 0)
	{
		GetWorldTimerManager().ClearTimer(Ability1_CooldownTimer);
		bAbility1_CanTrigger = true;
	}
}
void ABit::TickAbility2Cooldown()
{
	// Every tick decrement charge (this charges the ability since charge value is inverted)
	Ability2_Charge -= Ability2_ChargeRate;
	// If ability2 is charged (Ability2_Charge == 0) clear cooldown timer and enable ability
	if (Ability2_Charge == 0)
	{
		GetWorldTimerManager().ClearTimer(Ability2_CooldownTimer);
		bAbility2_CanTrigger = true;
	}
}
void ABit::TickAbility2Duration()
{
	// Ever tick decrement time remaining varaible
	Ability2_TimeRemaining -= 1;
	// If ability2 is done firing clear duration timer, reset immunity/time-remaining variables, start cooldown timer
	if (Ability2_TimeRemaining == 0)
	{
		GetWorldTimerManager().ClearTimer(Ability2_DurationTimer);
		Ability2_TimeRemaining = Ability2_Duration;
		bImmune = false;
		GetWorldTimerManager().SetTimer(Ability2_CooldownTimer, this, &ABit::TickAbility2Cooldown, 1.0f, true);
	}
}




