// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformGenerator.h"

// Sets default values
APlatformGenerator::APlatformGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlatformGenerator::BeginPlay()
{
	Super::BeginPlay();

	InitPerlinNoise();
}

float APlatformGenerator::Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float APlatformGenerator::Lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float APlatformGenerator::Grad(int hash, float x, float y)
{
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float APlatformGenerator::PerlinNoise(float x, float y)
{
	int X = (int)FMath::Floor(x) & 255;
	int Y = (int)FMath::Floor(y) & 255;

	x -= FMath::Floor(x);
	y -= FMath::Floor(y);

	float u = Fade(x);
	float v = Fade(y);

	int A = p[X] + Y;
	int AA = p[A];
	int AB = p[A + 1];
	int B = p[X + 1] + Y;
	int BA = p[B];
	int BB = p[B + 1];

	return Lerp(v, Lerp(u, Grad(p[AA], x, y), Grad(p[BA], x - 1, y)), Lerp(u, Grad(p[AB], x, y - 1), Grad(p[BB], x - 1, y - 1)));
}

void APlatformGenerator::InitPerlinNoise()
{
	// Create a vector containing values from 0 to 255
	std::vector<int> permutationTable(256);
	std::iota(permutationTable.begin(), permutationTable.end(), 0);

	// Use a random device as a seed
	std::random_device rd;
	std::mt19937 rng(rd());

	// Shuffle the permutation table using the random engine
	std::shuffle(permutationTable.begin(), permutationTable.end(), rng);

	// Copy the shuffled permutation table to the p array
	for (int i = 0; i < 256; ++i) {
		p[i] = p[i + 256] = permutationTable[i];
	}
}

// Called every frame
void APlatformGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

