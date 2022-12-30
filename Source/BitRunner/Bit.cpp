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
}

// Called every frame
void ABit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles y direction movement
	{
		float TargetVelocity = MovementInput.Y * MovementSpeed;
		float VelocityDelta = TargetVelocity - (this->GetVelocity().Y);
		float AccelerationState = abs(TargetVelocity) > 0.01f ? Acceleration : Decceleration;
		float ForceMultiplier = VelocityDelta * AccelerationState;
		StaticMesh->AddForce(ForceMultiplier * GetActorRightVector());
	}
	// Handles jump
	{
		if (bJump && bCanJump)
		{
			bCanJump = false;
			StaticMesh->AddImpulse(GetActorUpVector() * JumpMultiplier);
		}
	}

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

void ABit::ToggleJumpOn()
{
	bJump = true;
}

void ABit::ToggleJumpOff()
{
	bJump = false;
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

void ABit::MoveY(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}
