#include "Generators/GenerateAlgorithms.h"

#include <glm/vec3.hpp>
#include <noise/noise.h>
#include <glm/geometric.hpp>

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

	avg /= count;
	avg += random(randMax);
	arr[x][z] = avg;
}

void Generate::squareStepWrap(float** arr, int x, int z, int reach, const int maxSize, float randMax) {
	int count = 0;
	float avg = 0.f;

	if (x - reach >= 0) {
		avg += arr[x - reach][z];
	}
	else {
		avg += arr[maxSize - reach][z];
	}

	if (x + reach < maxSize) {
		avg += arr[x + reach][z];
	}
	else {
		avg += arr[0 + reach][z];
	}

	if (z - reach >= 0) {
		avg += arr[x][z - reach];
	}
	else {
		avg += arr[x][maxSize - reach];
	}

	if (z + reach < maxSize) {
		avg += arr[x][z + reach];
	}
	else {
		avg += arr[x][0 + reach];
	}

	avg /= 4;
	avg += random(randMax);
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

	avg /= 4; 
	avg += random(randMax);
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

void Generate::PerlinNoise(float** arr, int maxSize, int octaves, float h, float minHeight, float maxHeight) {

	for (int x = 0; x < maxSize; x++) {
		for (int z = 0; z < maxSize; z++) {
			double nx = x / maxSize - 0.5, nz = z / maxSize - 0.5;

			float amplitude = 1.f;
			float scalar = 1.f;
			float divideSum = 0.f;
			float result = 0.f, outputHeight = 0.f;

			// Combine multiple frequencies
			for (int i = 0; i < octaves; i++) {
				result += amplitude * scalar * nx * scalar* nz * 0.0;
				divideSum += amplitude;
				amplitude *= 0.5f;
				scalar *= 2.f;
			}

			// Average the noise values
			result /= divideSum;

			// flatten or stretch valleys
			result = powf(result, h);
			result = roundf(result * 512.f) / 512.f;

			float delta = maxHeight - minHeight;
			outputHeight = delta * result + minHeight;

			arr[x][z] = outputHeight;
			
		}
	}
}

float Generate::random(float range) {
	int wholeNumberRange = range * 1000 + 1; // Scale range by the thousands for higher precision

	return (rand() % wholeNumberRange) / 500.f - range;
}

float Generate::randomPositive(float range /*= 1.f*/) {
	int wholeNumberRange = range * 1000 + 1; // Scale range by the thousands for higher precision

	return (rand() % wholeNumberRange) / 1000.f;
}

float Generate::randomInRange(float low, float high) {
	int wholeNumberRange = (high - low) * 1000 + 1; // Scale range by the thousands for higher precision

	return (rand() % wholeNumberRange) / 1000.f + low;
}

void Generate::setRandomSeed(unsigned int seed) {
	srand(seed);
}
