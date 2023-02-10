// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "Bit.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AProjectile;

UCLASS()
class BITRUNNER_API ABit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABit();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Input
	UPROPERTY(EditAnywhere, Category = Input)
		UInputMappingContext* InputMapping;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* FireAbilityAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* Ability1_Action;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* Ability2_Action;

	// Components
	UPROPERTY(EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
		USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
		UCameraComponent* Camera;

	// Movement
	UPROPERTY(EditAnywhere, Category = Movement)
		float MinY;
	UPROPERTY(EditAnywhere, Category = Movement)
		float MaxY;
	UPROPERTY(VisibleAnywhere, Category = Movement)
		float MovementInput;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float MovementSpeedX;
	UPROPERTY(EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float MovementSpeedY;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bXMovementEnabled;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bYMovementEnabled;
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

	// Health
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"))
		float MaxHealth = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"))
		float CurrentHealth = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"))
		bool bImmune = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"))
		bool bIsDead = false;

	// Abilities
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bFireAbility_Triggered;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility1_Triggered;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility2_Triggered;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bFireAbility_CanTrigger = true;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility1_CanTrigger = true;
	UPROPERTY(VisibleAnywhere, Category = Abilities)
		bool bAbility2_CanTrigger = true;
	UPROPERTY(EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> FireProjectile;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		float Ability1_Charge = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		float Ability2_Charge = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		float Ability1_ChargeRate = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		float Ability2_ChargeRate = 10.0f;
	UPROPERTY()
		TArray<FHitResult> Ability1_Hits;
	UPROPERTY(EditAnywhere, Category = Abilities)
		double Ability1_Range = 100;
	UPROPERTY(EditAnywhere, Category = Abilities)
		float Ability2_Duration = 8.0f;
	UPROPERTY()
		float Ability2_TimeRemaining = Ability2_Duration;
	void TickAbility1Cooldown();
	void TickAbility2Cooldown();
	void TickAbility2Duration();
	FTimerHandle Ability1_CooldownTimer;
	FTimerHandle Ability2_CooldownTimer;
	FTimerHandle Ability2_DurationTimer;
	void TriggerFireAbility(const FInputActionValue& Value);
	void TriggerAbility1(const FInputActionValue& Value);
	void TriggerAbility2(const FInputActionValue& Value);
	
};
