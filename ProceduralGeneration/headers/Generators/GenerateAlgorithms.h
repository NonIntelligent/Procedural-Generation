#pragma once

enum class Algorithm {
	DIAMOND_SQUARE, PERLIN_NOISE
};

class Generate {
	Generate() {};

// https://medium.com/@nickobrien/diamond-square-algorithm-explanation-and-c-implementation-5efa891e486f

	// Set the midpoint of the diamond to the avg of 4 values to form a square
	static void squareStep(float** arr, int x, int z, int reach, const int maxSize);
	// Set the midpoint of a diamond shape to eventually leave behind a square
	static void diamondStep(float** arr, int x, int z, int reach, const int maxSize);

	// Used to add a random value to the result point of the diamond or square step
	// @param h A value between 0.0 and 0.1 to add roughness (2^-h)
	static float diamondRandom(float h);

public:
	// Modifies the array based on the Diamond Square algorithm
	// @param size The size of the array in both dimensions
	static void DiamondSquare(float** arr, int size, const int maxSize);

	static float random();

	static void setRandomSeed(unsigned int seed);
};