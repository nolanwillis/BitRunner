// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "Bit.generated.h"

class UInputMappingContext;
class UInputAction;

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
	UPROPERTY(EditAnywhere, Category = Camera)
		class USpringArmComponent* SpringArm;

	// Camera component
	UPROPERTY()
		class UCameraComponent* Camera;

	// Input
	UPROPERTY (EditAnywhere, Category = Input)
		UInputMappingContext* InputMapping;
	UPROPERTY (EditAnywhere, Category = Input)
		UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* Ability1Action;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* Ability2Action;
	 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Movement
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement)
		float MovementSpeedX;

	// Health 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health)
		float MaxHealth = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health)
		float CurrentHealth = 100.0f;

	// Abilities
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities)
		float Ability1_Charge = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities)
		float Ability2_Charge = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities)
		float Ability1_ChargeRate = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities)
		float Ability2_ChargeRate = 10.0f;

private:
	// Movement
	UPROPERTY(EditAnywhere, Category = Movement)
		float MovementSpeedY;
	UPROPERTY(EditAnywhere, Category = Movement)
		bool bXMovementEnabled;
	UPROPERTY(VisibleAnywhere, Category = Movement)
		float MovementInput;
	void Move(const FInputActionValue& Value);

	// Jumping
	UPROPERTY(VisibleAnywhere, Category = Jump)
		bool bJump;
	UPROPERTY(EditAnywhere, Category = Jump)
		float JumpHeight;
	UPROPERTY(EditAnywhere, Category = Jump)
		bool bCanJump = true;
	UPROPERTY(EditAnywhere, Category = Jump)
		UCurveFloat* JumpHeightCurve;
	UPROPERTY(EditAnywhere, Category = Jump)
		UCurveFloat* JumpTwistCurve;
	UPROPERTY(VisibleAnywhere, Category = Jump)
		FVector JumpStartLoc;
	UPROPERTY(VisibleAnywhere, Category = Jump)
		FVector JumpEndLoc;
	UPROPERTY(VisibleAnywhere, Category = Jump)
		FRotator JumpStartRot;
	UPROPERTY(VisibleAnywhere, Category = Jump)
		FRotator JumpEndRot;
	UPROPERTY()
	FTimeline JumpTimeline;
	void Jump(const FInputActionValue& Value);
	UFUNCTION()
		void JumpHeightUpdate(float Value);
	UFUNCTION() 
		void JumpHeightFinished();
	UFUNCTION()
		void JumpTwistUpdate(float Value);

	// Abilities
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility1_Triggered;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility2_Triggered;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility1_CanTrigger = true;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility2_CanTrigger = true;
	UPROPERTY()
		TArray<FHitResult> Ability1_Hits;
	UPROPERTY(EditAnywhere, Category = Abilities)
		double Ability1_Range = 100;
	void TickAbility1Timer();
	void TickAbility2Timer();
	FTimerHandle Ability1_TimerHandle;
	FTimerHandle Ability2_TimerHandle;
	void TriggerAbility1(const FInputActionValue& Value);
	void TriggerAbility2(const FInputActionValue& Value);
};
