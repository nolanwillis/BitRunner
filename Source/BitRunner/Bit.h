// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bit.generated.h"

UCLASS()
class BITRUNNER_API ABit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;
	
	// Action callbacks
	void ToggleJumpOn();

	void ToggleJumpOff();

	void ToggleAbility1On();

	void ToggleAbility1Off();

	void ToggleAbility2On();

	void ToggleAbility2Off();
	
	// Axis callbacks
	void MoveY(float AxisValue);

	// Input variables
	UPROPERTY(VisibleAnywhere)
	FVector2D MovementInput;
	UPROPERTY(VisibleAnywhere)
	bool bJump;
	UPROPERTY(VisibleAnywhere)
	bool bAbility1Triggered;
	UPROPERTY(VisibleAnywhere)
	bool bAbility2Triggered;

	// Movement variables
	UPROPERTY(EditAnywhere);
	float MovementSpeed;
	UPROPERTY(EditAnywhere);
	float Acceleration; 
	UPROPERTY(EditAnywhere);
	float Decceleration;
	UPROPERTY(EditAnywhere);
	float JumpMultiplier;
	UPROPERTY(EditAnywhere);
	bool bCanJump;
	

	// Temporary camera variable
	UPROPERTY(EditAnywhere);
	AActor* Camera;
	
};
