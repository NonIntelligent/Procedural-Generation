#include "Generator.h"

#include <iostream>

bool Generator::initSDL() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	else {
		//create the window
		window = SDL_CreateWindow("SDL Version", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scene.getWidth(), scene.getHeight(), SDL_WINDOW_SHOWN);
		if(window == NULL) {
			std::cout << "SDL Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
			return false;
		}
		else {
			surface = SDL_GetWindowSurface(window);
			return true;
		}
	}
}

Generator::Generator() {
	cameraPos = vec3(2.f, 0.f, 0.f);
	lookAtDir = vec3(0.f, 0.f, -1.f);
	g_fov = 40.f;
	threads.reserve(threadCount);
}

Generator::~Generator() {
	scene.deleteAllObjects();
	scene.deleteResources();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Generator::init() {

	return initSDL();
}

// used for camera movement
vec3 movementDir = vec3(0);
vec3 lightMovementDir = vec3(0);
float deltaX = 0, deltaY = 0;
bool stats = false;

void Generator::handleInputs() {
	SDL_Event SDLevent;
	while(SDL_PollEvent(&SDLevent) != 0) {
		int keyCode = SDLevent.key.keysym.sym;
		switch(SDLevent.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			movementDir.x = keyCode == SDLK_a ? -1 : keyCode == SDLK_d ? 1 : movementDir.x;
			movementDir.y = keyCode == SDLK_LCTRL ? -1 : keyCode == SDLK_SPACE ? 1 : movementDir.y;
			movementDir.z = keyCode == SDLK_w ? -1 : keyCode == SDLK_s ? 1 : movementDir.z;

			lightMovementDir.x = keyCode == SDLK_LEFT ? -1 : keyCode == SDLK_RIGHT ? 1 : lightMovementDir.x;
			lightMovementDir.y = keyCode == SDLK_DOWN ? -1 : keyCode == SDLK_UP ? 1 : lightMovementDir.y;
			lightMovementDir.z = keyCode == SDLK_COMMA ? -1 : keyCode == SDLK_PERIOD ? 1 : lightMovementDir.z;

			if(keyCode == SDLK_F3) stats = !stats;

			break;

		case SDL_KEYUP:
			if(keyCode == SDLK_ESCAPE) {
				running = false;
				break;
			}
			if(keyCode == SDLK_g) {
				
			}
			if(keyCode == SDLK_a || keyCode == SDLK_d) {
				movementDir.x = 0;
			}
			if(keyCode == SDLK_LCTRL || keyCode == SDLK_SPACE) {
				movementDir.y = 0;
			}
			if(keyCode == SDLK_w || keyCode == SDLK_s) {
				movementDir.z = 0;
			}

			if(keyCode == SDLK_LEFT || keyCode == SDLK_RIGHT) {
				lightMovementDir.x = 0;
			}
			if(keyCode == SDLK_DOWN || keyCode == SDLK_UP) {
				lightMovementDir.y = 0;
			}
			if(keyCode == SDLK_COMMA || keyCode == SDLK_PERIOD) {
				lightMovementDir.z = 0;
			}
			break;

		case SDL_MOUSEWHEEL:
			if(SDLevent.wheel.y > 0) {
				g_fov -= 2;
			}
			else if(SDLevent.wheel.y < 0) {
				g_fov += 2;
			}
			break;
		default:
			break;
		}

	}
}

void Generator::update() {

}

void Generator::render() {
	SDL_UpdateWindowSurface(window);
}

void Generator::mainLoop() {
	// variables needed to run the game application loop
	running = true;
	std::chrono::nanoseconds nsPerRenderLimit(1000000000L / targetFps); // todo check targetFps != 0

	int tps = 0, fps = 0, ticks = 0, frames = 0;

	float averageTimeToRenderFrame = 0.f;


	auto previousTime = high_res_clock::now(), currentTime = high_res_clock::now();

	//std::chrono::duration<__int64, std::nano> physicsTimer, frameTimer, timer;
	nanoSeconds loopTime(0ns), physicsTimer(0ns), frameTimer(0ns), timer(0ns), currentTimeInNano(0ns);

	frameTimer = currentTime.time_since_epoch();
	timer = currentTime.time_since_epoch();

	// Loop starts here
	while(running) {
		// How much time has passed since the last frame
		currentTime = high_res_clock::now();
		currentTimeInNano = currentTime.time_since_epoch(); // Store return value

		loopTime = currentTime - previousTime;

		previousTime = currentTime;
		// time accumulator
		physicsTimer += loopTime;

		handleInputs();

		// Do multiple physics ticks when behind
		while(physicsTimer >= NS_PER_TICK) {
			update();
			ticks++;
			physicsTimer -= NS_PER_TICK;
		}

		// Render once per loop at a set cap.
		// NOTE Turning on v-sync will also limit the amount of times this if statement is called
		if(currentTimeInNano - frameTimer >= nsPerRenderLimit) {
			uint64 start = SDL_GetPerformanceCounter();
			render(); // Render in between physics updates
			uint64 end = SDL_GetPerformanceCounter();

			averageTimeToRenderFrame += (end - start) / (float)SDL_GetPerformanceFrequency();

			frames++;
			frameTimer += nsPerRenderLimit;
		}

		// Do stuff every second
		if(currentTimeInNano - timer >= singleSecond) {
			tps = ticks;
			fps = frames;

			std::cout << "tps: " << tps << " fps: " << fps << std::endl;
			std::cout << "time to render = " << (averageTimeToRenderFrame * 1000) / frames << "ms" << std::endl;

			if(frameSampleIndex == 60) {
				float tenSecondAverage = 0.f;
				for(int i = 0; i < 60; i++) {
					tenSecondAverage += frameSamples[i];
				}
				tenSecondAverage /= 60;

				std::cout << "average time to render last 60 seconds = " << (tenSecondAverage) / frames << "s" << std::endl;

				frameSampleIndex = 0;
			}

			frameSamples[frameSampleIndex] = averageTimeToRenderFrame;
			frameSampleIndex++;

			ticks = 0;
			frames = 0;
			averageTimeToRenderFrame = 0.f;

			timer += singleSecond;
		}

	}
}
