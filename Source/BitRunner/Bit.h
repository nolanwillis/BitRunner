// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "Bit.generated.h"

class UCurveFloat;

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
	 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Movement
	UPROPERTY(VisibleAnywhere, Category = "Input") 
		FVector2D MovementInput;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		float MovementSpeed;

	// Jumping
	UPROPERTY(VisibleAnywhere, Category = "Input") 
		bool bJump;
	UPROPERTY(EditAnywhere, Category = "Bit Settings") 
		float JumpHeight;
	UPROPERTY(EditAnywhere, Category = "Input") 
		bool bCanJump;
	UPROPERTY(EditAnywhere, Category = "Bit Settings")
		UCurveFloat* JumpingCurve;
	UPROPERTY() 
		FVector JumpStartLoc;
	UPROPERTY() 
		FVector JumpEndLoc;

	// Abilities
	UPROPERTY(VisibleAnywhere, Category = "Input")
		bool bAbility1Triggered;
	UPROPERTY(VisibleAnywhere, Category = "Input")
		bool bAbility2Triggered;

	// Camera
	UPROPERTY(EditAnywhere, Category = "Camera") 
		AActor* Camera;

private:
	// Movement
	void MoveY(float AxisValue);

	// Jumping
	FTimeline JumpingTimeline;
	void ToggleJumpOn();
	void ToggleJumpOff();
	UFUNCTION()
		void JumpUpdate(float Value);
	UFUNCTION() 
		void JumpFinished();

	// Abilities
	void ToggleAbility1On();
	void ToggleAbility1Off();
	void ToggleAbility2On();
	void ToggleAbility2Off();
};
