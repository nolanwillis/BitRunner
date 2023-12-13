// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;

UCLASS()
class BITRUNNER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
		UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
		UParticleSystemComponent* ProjectileParticleSystem;
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UPROPERTY(EditAnywhere, Category = Properties)
		float Damage = 50.0f;
	UPROPERTY(EditAnywhere, Category = Properties)
		float Speed = 3000.0f;
	UPROPERTY(EditAnywhere, Category = Properties)
		UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, Category = Properties)
		USoundBase* LaunchSound;
	UPROPERTY(EditAnywhere, Category = Properties)
		USoundBase* HitSound;
};
