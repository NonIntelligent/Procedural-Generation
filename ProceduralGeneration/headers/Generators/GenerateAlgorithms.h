#pragma once

enum class Algorithm {
	DIAMOND_SQUARE, PERLIN_NOISE
};

class Generate {
	Generate() {};

// https://medium.com/@nickobrien/diamond-square-algorithm-explanation-and-c-implementation-5efa891e486f

	// Set the midpoint of a diamond to the avg of 4 values to later form a square
	static void squareStep(float** arr, int x, int z, int reach, const int maxSize, float randMax);
	// Set the midpoint of a square to the avg of 4 values to later form a diamond
	static void diamondStep(float** arr, int x, int z, int reach, const int maxSize, float randMax);

public:
	// Modifies the array based on the Diamond Square algorithm
	// @param size length of the current square grid (initial = maxSize)
	// @param maxSize the size of the array in both dimensions
	static void DiamondSquare(float** arr, int size, const int maxSize, float randMax, const float h);

	static void PerlinNoise(float** arr, int size);

	// Generate a random value within the values (-range -> +range) with accuracy of 3 decimal places
	static float random(float range = 1.f);

	static void setRandomSeed(unsigned int seed);
};