#pragma once
#include <glm/vec2.hpp>

class Generate {
	Generate() {};

// https://medium.com/@nickobrien/diamond-square-algorithm-explanation-and-c-implementation-5efa891e486f

	// Set the midpoint of a diamond to the avg of 3-4 values to later form a square
	static void squareStep(float** arr, int x, int z, int reach, const int maxSize, float randMax);
	// Same as squareStep but will wrap around the array to always sample 4 values
	static void squareStepWrap(float** arr, int x, int z, int reach, const int maxSize, float randMax);

	// Set the midpoint of a square to the avg of 4 values to later form a diamond
	static void diamondStep(float** arr, int x, int z, int reach, const int maxSize, float randMax);

public:
	// Modifies the array based on the Diamond Square algorithm
	// @param size length of the current square grid (initial = maxSize)
	// @param maxSize the size of the array in both dimensions
	static void DiamondSquare(float** arr, int size, const int maxSize, float randMax, const float h);

	// https://gpfault.net/posts/perlin-noise.txt.html
	// https://www.redblobgames.com/maps/terrain-from-noise/

	static void PerlinNoise(float** arr, int maxSize, int octaves, float h, float minHeight, float maxHeight);

	// Generate a random value within the values (-range -> +range) with accuracy of 3 decimal places
	static float random(float range = 1.f);
	// Generate a random value between (0 -> +range) with accuracy of 3 decimal places
	static float randomPositive(float range = 1.f);

	static float randomInRange(float low, float high);

	static void setRandomSeed(unsigned int seed);
};