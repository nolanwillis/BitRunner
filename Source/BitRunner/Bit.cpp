// Fill out your copyright notice in the Description page of Project Settings.

#include "Bit.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABit::ABit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// Attach components
	StaticMesh->SetupAttachment(RootComponent);
	
	// Set default player controller to posses pawn
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABit::BeginPlay()
{
	Super::BeginPlay();
	
	// Set current camera
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		PlayerController->SetViewTarget(Camera);
	}
	if (JumpingCurve)
	{
		// Bind jump timeline functions
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this, FName("JumpUpdate"));
		FOnTimelineEvent FinishedEvent;
		FinishedEvent.BindUFunction(this, FName("JumpFinished"));

		// Add interp float and finish function to jumping timeline
		JumpingTimeline.AddInterpFloat(JumpingCurve, ProgressUpdate);
		JumpingTimeline.SetTimelineFinishedFunc(FinishedEvent);

		JumpStartLoc = JumpEndLoc = GetActorLocation();
		JumpEndLoc.Z += JumpHeight;
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
			MovementInput = MovementInput.GetSafeNormal() * MovementSpeed;
			FVector NewLocation = GetActorLocation();
			NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
			SetActorLocation(NewLocation);
			// Update y component of the jump start/end location 
			JumpStartLoc.Y = GetActorLocation().Y;
			JumpEndLoc.Y = GetActorLocation().Y;
		}
	}
	// If timeline is played tick it using delta time
	JumpingTimeline.TickTimeline(DeltaTime);
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
		JumpingTimeline.PlayFromStart();
	}
}
void ABit::ToggleJumpOff()
{
	bJump = false;
}
void ABit::JumpUpdate(float Value) 
{
	FVector NewLocation = FMath::Lerp(JumpStartLoc, JumpEndLoc, Value);
	SetActorLocation(NewLocation);
}
void ABit::JumpFinished()
{
	bCanJump = true;
}

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


