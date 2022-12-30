// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TimelineComponent.h"
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

	// Static mesh component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMesh;
	 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Input callbacks
	void ToggleJumpOn();

	void ToggleJumpOff();

	void ToggleAbility1On();

	void ToggleAbility1Off();

	void ToggleAbility2On();

	void ToggleAbility2Off();

	void MoveY(float AxisValue);
	
	// Jump timeline functions
	UFUNCTION() 
		void TimelineFinished();
	UFUNCTION() void 
		TimelineProgress(float Value);
	
	
	UPROPERTY(VisibleAnywhere, Category = 'Input') 
		FVector2D MovementInput;
	UPROPERTY(VisibleAnywhere, Category = 'Input') 
		bool bJump;
	UPROPERTY(VisibleAnywhere, Category = 'Input') 
		bool bAbility1Triggered;
	UPROPERTY(VisibleAnywhere, Category = 'Input') 
		bool bAbility2Triggered;
	UPROPERTY(EditAnywhere, Category = 'Locomotion') 
		float MovementSpeed;
	UPROPERTY(EditAnywhere, Category = 'Jump') 
		float JumpHeight;
	UPROPERTY(EditAnywhere, Category = 'Jump') 
		bool bCanJump;
	
	FTimeline CurveFTimeline;
	UPROPERTY()
		UCurveFloat* CurveFloat;
	UPROPERTY() 
		FVector JumpStartLoc;
	UPROPERTY() 
		FVector JumpEndLoc;

	// Temperary camera variable
	UPROPERTY(EditAnywhere, Category = 'Camera'); AActor* Camera;



	/* UPROPERTY(EditAnywhere) float JumpMultiplier;
	UPROPERTY(EditAnywhere) float Acceleration;
	UPROPERTY(EditAnywhere) float Decceleration;*/
	
};
