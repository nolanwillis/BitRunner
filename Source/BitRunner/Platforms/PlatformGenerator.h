// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include <algorithm>
#include <numeric>
#include <random>
#include "PlatformGenerator.generated.h"

UCLASS()
class BITRUNNER_API APlatformGenerator : public AActor
{
	GENERATED_BODY()

public:
	inline static int p[512] = { /* Permutation table */ };
	
	// Sets default values for this actor's properties
	APlatformGenerator();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	float PerlinNoise(float x, float y);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	float Fade(float t);
	float Lerp(float t, float a, float b);
	float Grad(int hash, float x, float y);
	void InitPerlinNoise();
};
