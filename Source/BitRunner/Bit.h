// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
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

	// Static mesh component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMesh;

	// Spring arm component
	UPROPERTY(EditAnywhere)
		class USpringArmComponent* SpringArm;

	// Camera component
	UPROPERTY()
		class UCameraComponent* Camera;
	 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	

private:
	// Movement
	UPROPERTY(VisibleAnywhere, Category = "Input")
		FVector2D MovementInput;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		float MovementSpeedY;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		float MovementSpeedX;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		bool bXMovementEnabled;
	void MoveY(float AxisValue);

	// Jumping
	UPROPERTY(VisibleAnywhere, Category = "Input")
		bool bJump;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		float JumpHeight;
	UPROPERTY(EditAnywhere, Category = "Input")
		bool bCanJump;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		UCurveFloat* JumpHeightCurve;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		UCurveFloat* JumpTwistCurve;
	UPROPERTY(VisibleAnywhere, Category = "Bit Settings")
		FVector JumpStartLoc;
	UPROPERTY(VisibleAnywhere, Category = "Bit Settings")
		FVector JumpEndLoc;
	UPROPERTY(VisibleAnywhere, Category = "Bit Settings")
		FRotator JumpStartRot;
	UPROPERTY(VisibleAnywhere, Category = "Bit Settings")
		FRotator JumpEndRot;
	UPROPERTY()
	FTimeline JumpTimeline;
	void ToggleJumpOn();
	void ToggleJumpOff();
	UFUNCTION()
		void JumpHeightUpdate(float Value);
	UFUNCTION() 
		void JumpHeightFinished();
	UFUNCTION()
		void JumpTwistUpdate(float Value);

	// Abilities
	UPROPERTY(VisibleAnywhere, Category = "Input")
		bool bAbility1Triggered;
	UPROPERTY(VisibleAnywhere, Category = "Input")
		bool bAbility2Triggered;
	void ToggleAbility1On();
	void ToggleAbility1Off();
	void ToggleAbility2On();
	void ToggleAbility2Off();

	
};
