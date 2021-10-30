#include "Generators/GenerateAlgorithms.h"

#include <cstdlib>

void Generate::squareStep(float** arr, int x, int z, int reach, const int maxSize) {
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

	avg += diamondRandom(0.1f);
	avg /= count; // Can count be 0?, only if supplied values 
	arr[x][z] = avg;
}

void Generate::diamondStep(float** arr, int x, int z, int reach, const int maxSize) {
	int count = 0;
	float avg = 0.f;
	// Check if the point to sample is within bounds of the array

	if (x - reach >= 0 && z - reach >= 0) {
		avg += arr[x - reach][z - reach];
		count++;
	}

	if (x - reach >= 0 && z + reach < maxSize) {
		avg += arr[x - reach][z + reach];
		count++;
	}

	if (x + reach < maxSize && z - reach >= 0) {
		avg += arr[x + reach][z - reach];
		count++;
	}

	if (x + reach < maxSize && z + reach < maxSize) {
		avg += arr[x + reach][z + reach];
		count++;
	}

	avg += diamondRandom(0.1f);
	avg /= count; // Can count be 0?, only if supplied values 
	arr[x][z] = avg;
}

float Generate::diamondRandom(float h) {
	return random() * powf(2, -h);
}

void Generate::DiamondSquare(float** arr, int size, const int maxSize) {
	int midpoint = size / 2;
	if (midpoint < 1) return;

	for (int x = midpoint; x < maxSize; x += size) {
		for (int z = midpoint; z < maxSize; z += size) {
			diamondStep(arr, x % maxSize, z % maxSize, midpoint, maxSize);
		}
	}

	int currentColumn = 0;

	for (int x = 0; x < maxSize; x += midpoint) {
		currentColumn++;

		// Odd column
		if (currentColumn % 2 == 1) {
			for (int z = midpoint; z < maxSize; z += midpoint) {
				squareStep(arr, x % maxSize, z % maxSize, midpoint, maxSize);
			}
		}
		else {
			for (int z = 0; z < maxSize; z += midpoint) {
				squareStep(arr, x % maxSize, z % maxSize, midpoint, maxSize);
			}
		}
	}

	DiamondSquare(arr, midpoint, maxSize);
}

float Generate::random() {
	return (rand() % 1001) / 500.f - 1.f;
}

void Generate::setRandomSeed(unsigned int seed) {
	srand(seed);
}
