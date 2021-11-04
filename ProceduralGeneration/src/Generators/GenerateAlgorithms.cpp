#include "Generators/GenerateAlgorithms.h"

#include <cstdlib>
#include <iostream>

void Generate::squareStep(float** arr, int x, int z, int reach, const int maxSize, float randMax) {
	int count = 0;
	float avg = 0.f;
	// Check if the point to sample is within bounds of the array

	if (x - reach >= 0) {
		avg += arr[x - reach][z];
		count++;
	}

	if (x + reach < maxSize) {
		avg += arr[x + reach][z];
		count++;
	}

	if (z - reach >= 0) {
		avg += arr[x][z - reach];
		count++;
	}

	if (z + reach < maxSize) {
		avg += arr[x][z + reach];
		count++;
	}

	avg += random(randMax);
	avg /= count;
	arr[x][z] = avg;
}

void Generate::diamondStep(float** arr, int x, int z, int reach, const int maxSize, float randMax) {
	int count = 0;
	float avg = 0.f;
	// Check if the point to sample is within bounds of the array

	avg += arr[x - reach][z - reach];

	avg += arr[x - reach][z + reach];

	avg += arr[x + reach][z - reach];

	avg += arr[x + reach][z + reach];

	avg += random(randMax);
	avg /= 4; 
	arr[x][z] = avg;
}

void Generate::DiamondSquare(float** arr, int size, const int maxSize, float randMax,const float h) {
	static int diamondStepCalls = 0;
	static int squareStepCalls = 0;

	int midpoint = size / 2;
	if (midpoint < 1) return;

	for (int x = midpoint; x < maxSize; x += size) {
		for (int z = midpoint; z < maxSize; z += size) {
			diamondStep(arr, x % maxSize, z % maxSize, midpoint, maxSize, randMax);
			diamondStepCalls++;
		}
	}

	int currentColumn = 0;

	for (int x = 0; x < maxSize; x += midpoint) {
		currentColumn++;

		// Odd column
		if (currentColumn % 2 == 1) {
			for (int z = midpoint; z < maxSize; z += size) {
				squareStep(arr, x % maxSize, z % maxSize, midpoint, maxSize, randMax);
				squareStepCalls++;
			}
		}
		else {
			for (int z = 0; z <= maxSize; z += size) {
				squareStep(arr, x % maxSize, z % maxSize, midpoint, maxSize, randMax);
				squareStepCalls++;
			}
		}
	}

	DiamondSquare(arr, midpoint, maxSize, randMax * powf(2, -h), h);
}

float Generate::random(float range) {
	int wholeNumberRange = range * 1000 + 1; // Scale range by the thousands for higher precision

	return (rand() % wholeNumberRange) / 500.f - (float) range;
}

void Generate::setRandomSeed(unsigned int seed) {
	srand(seed);
}
