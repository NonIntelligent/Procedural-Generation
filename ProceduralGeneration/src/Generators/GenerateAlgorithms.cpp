#include "Generators/GenerateAlgorithms.h"

#include <glm/vec3.hpp>
#include <glm/detail/func_common.inl>
#include <glm/gtc/noise.hpp>

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

glm::vec2 Generate::gradient(glm::vec2 point) {

}

float Generate::noise(float a, float b) {
	glm::vec2 p0 = glm::floor(glm::vec2(a, b));
	glm::vec2 p1 = p0 + glm::vec2(1.f, 0.f);
	glm::vec2 p2 = p0 + glm::vec2(0.f, 1.f);
	glm::vec2 p3 = p0 + glm::vec2(1.f, 1.f);

	glm::vec2 g0 = gradient(p0);
	glm::vec2 g1 = gradient(p1);
	glm::vec2 g2 = gradient(p2);
	glm::vec2 g3 = gradient(p3);

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

void Generate::PerlinNoise(float** arr, int maxSize, int baseFrequency, float h) {
	for (int x = 0; x < maxSize; x++) {
		for (int z = 0; z < maxSize; z++) {
			float nx = (float) x / maxSize - 0.5f, nz = (float) z / maxSize - 0.5f;

			float amplitude = 1.f;
			float scalar = 1.f;
			float result = 0.f;

			// Combine multiple frequencies
			for (int i = 0; i < 4; i++) {
				result += amplitude * glm::perlin(glm::vec2{baseFrequency * scalar * nx, baseFrequency * scalar * nz});
				amplitude *= 0.5f;
				scalar *= 2.f;
			}

			// Average the noise values
			result /= 1.f + 0.5f + 0.25f + 0.125f;

			arr[x][z] = exp2f(h); // flatten or stretch valleys
			
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

void Generate::setRandomSeed(unsigned int seed) {
	srand(seed);
}
