#pragma once
#include "macros/Macro_chrono.h"
#include "Scene.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <SDL/SDL.h>
#include <thread>
#include <vector>

const int CORE_COUNT = std::thread::hardware_concurrency();

using namespace glm;

class Generator {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* surface = nullptr;

	// update rate of 30hz
	const nanoSeconds NS_PER_TICK = nanoSeconds(32222us);
	long targetFps = 8000;
	volatile bool running = false;
	const int threadCount = CORE_COUNT == 0 ? 12 : CORE_COUNT;
	float frameSamples[60] = {0.f};
	int frameSampleIndex = 0;
	std::vector<std::thread> threads;

	// Objects
	Scene scene;

	// Camera options
	vec3 cameraPos;
	vec3 lookAtDir;
	float horizontalAngle, verticalAngle, g_fov;
	const float cameraSpeed = 2.f;

	bool initSDL();

public:
	Generator();
	~Generator();

	bool init();

	void handleInputs();
	void update();
	void render();

	void mainLoop();
};

