// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	// Set default values for projectile movement component
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->InitialSpeed = Speed;

	// Attach components
	StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Bind callback to OnComponentHit event
	StaticMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	// Play launch sound
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Variable that stores the owner of a given projectile
	AActor* MyOwner = GetOwner();

	// Checks if MyOwner is null
	if (MyOwner == nullptr)
	{
		Destroy();
		return;
	}

	// Variable that stores the instigator controller of the owner of a given projectile
	auto MyOwnerInstigator = MyOwner->GetInstigatorController();

	// Variable that holds the damage type class (not using damage type in this game)
	auto DamageTypeClass = UDamageType::StaticClass();

	// If a projectile hits an actor that isn't a tank
	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		// Spawns emitter at hit location for P_HitEffect particle system (if HitParticle != nullptr)
		if (HitParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
		}
		// Applies damage
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
		// Plays hit sound
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}
	}
	// Destroys projectile 
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

