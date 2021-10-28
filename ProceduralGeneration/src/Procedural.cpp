#include "Procedural.h"

#include <iostream>

// Error callback for GLFW
void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

bool Procedural::initGLFW() {
	glfwSetErrorCallback(&error_callback);

	if(!glfwInit()) {
		std::cout << "GLFW could not initialize! " << std::endl;
		return false;
	}

	// Change openGL context and other hints
	setupWindowHints();

	//create the window
	window = glfwCreateWindow(1280, 720, "Procedural-Generation", NULL, NULL);
	if(!window) {
		std::cout << "GLFW Window could not be created!" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	return true;
}

void Procedural::setupWindowHints() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

// used for camera movement
vec3 movementDir = vec3(0);
vec3 lightMovementDir = vec3(0);
float deltaX = 0, deltaY = 0;
bool stats = false;

void Procedural::updateKeyboardInput() {

	int stateW = glfwGetKey(window, GLFW_KEY_W);
	int stateA = glfwGetKey(window, GLFW_KEY_A);
	int stateS = glfwGetKey(window, GLFW_KEY_S);
	int stateD = glfwGetKey(window, GLFW_KEY_D);

	int stateUp = glfwGetKey(window, GLFW_KEY_SPACE);
	int stateDown = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);

	movementDir.x = stateA == GLFW_PRESS ? -1.f : stateD == GLFW_PRESS ? 1.f : 0.f;
	movementDir.y = stateDown == GLFW_PRESS ? -1.f : stateUp == GLFW_PRESS ? 1.f : 0.f;
	movementDir.z = stateW == GLFW_PRESS ? -1.f : stateS == GLFW_PRESS ? 1.f : 0.f;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Procedural::updateMouseInput() {

}

Procedural::Procedural() {
	cameraPos = vec3(2.f, 0.f, 0.f);
	lookAtDir = vec3(0.f, 0.f, -1.f);
	g_fov = 40.f;
	threads.reserve(threadCount);
}

Procedural::~Procedural() {
	scene.deleteAllObjects();
	scene.deleteResources();

	for (auto& elem : shaders) {
		elem.second.deleteProgram();
	}
	shaders.clear();

	terrain.destroyTerrain();

	glfwTerminate();
}

bool Procedural::init() {
	bool glfw = initGLFW();
	GLenum error;

	if((error = glewInit()) != GLEW_OK) {
		std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
		glfwTerminate();
		return false;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPrimitiveRestartIndex(65535);
	glEnable(GL_PRIMITIVE_RESTART);

	projMat = perspective(radians(60.0), (double) scene.getWidth() / scene.getHeight(), 0.1, 100.0);

	createShaders();

	createObjects();

	return true;
}

void Procedural::createShaders() {
	Shader shader;
	shader.parseShader("res/Shaders/vertexShader.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader.createShader();

	shaders["terrain"] = shader;
}

void Procedural::createObjects(){
	terrain.init();
	terrain.setShaderName("terrain");
}

void Procedural::handleInputs() {
	glfwPollEvents();

	updateKeyboardInput();
}

void Procedural::update() {

}

void Procedural::render() {
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* current = nullptr;

	auto result = shaders.find("terrain");
	if (result != shaders.end()) {
		current = &(result->second);
	}

	current->bind();
	terrain.va->bind();
	
	updateShaderUniform(current, terrain.uniforms);

	terrain.ib->bind();

	GLCall(glDrawElements(GL_TRIANGLE_STRIP, terrain.ib->getCount(), GL_UNSIGNED_INT, nullptr));

	glfwSwapBuffers(window);
}

Texture Procedural::findTexture(const std::string& name) {
	Texture texture;

	for (auto it = textures.begin(); it != textures.end(); it++) {
		if (name == (*it).getName()) {
			texture = (*it);
			break;
		}
	}

	return texture;
}

void Procedural::updateShaderUniform(Shader* shader, const std::vector<ShaderUniform>& uniforms) {
	for (auto it = uniforms.begin(); it != uniforms.end(); it++) {
		if ((*it).name == "setup" || (*it).type == UniformType::NONE) {
			std::cout << "Uniform not setup, skipping." << std::endl;
			continue;
		}

		switch ((*it).type) {
		case UniformType::VEC3:
			shader->setUniform3f((*it).name, (*it).dataMatrix[0]);
			break;
		case UniformType::VEC4:
			shader->setUniform4f((*it).name, (*it).dataMatrix[0]);
			break;
		case UniformType::MAT4:
			shader->setUniformMatrix4fv((*it).name, 1, GL_FALSE, (*it).dataMatrix);
			break;
		case UniformType::INT1:
			shader->setUniform1i((*it).name, (*it).dataMatrix[0][0]);
			break;
		case UniformType::FLOAT1:
			shader->setUniform1f((*it).name, (*it).dataMatrix[0][0]);
			break;
		case UniformType::TEXTURE:
		{
			Texture texture = findTexture((*it).resourceName);
			// Texture could not be found
			if (texture.getName() == "error") {
				textures.at(0).bind(0);
				shader->setUniform1i((*it).name, 0);
				break;
			}

			unsigned int bindingPoint = (*it).dataMatrix[0][0];


			if (bindingPoint > 16) {
				textures.at(0).bind(0);
				shader->setUniform1i((*it).name, (int)bindingPoint);
				break;
			}

			texture.bind(bindingPoint);

			shader->setUniform1i((*it).name, (int)bindingPoint);

			break;
		}
		case UniformType::CUBEMAP:
		{
			Texture texture = findTexture((*it).resourceName);
			// Texture could not be found
			if (texture.getName() == "error") {
				break;
			}

			unsigned int bindingPoint = (*it).dataMatrix[0][0];
			GLCall(glActiveTexture(GL_TEXTURE0 + bindingPoint));
			GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, texture.getTextureID()));
			shader->setUniform1i((*it).name, (int)bindingPoint);

			break;
		}
		default:
			printf("Uniform type not supported");
			break;
		}
	}
}

void Procedural::mainLoop() {
	// variables needed to run the game application loop
	running = true;
	std::chrono::nanoseconds nsPerRenderLimit(1000000000L / targetFps); // todo check targetFps != 0

	int tps = 0, fps = 0, ticks = 0, frames = 0;

	double averageTimeToRenderFrame = 0.0;


	auto previousTime = high_res_clock::now(), currentTime = high_res_clock::now();

	//std::chrono::duration<__int64, std::nano> physicsTimer, frameTimer, timer;
	nanoSeconds loopTime(0ns), physicsTimer(0ns), frameTimer(0ns), timer(0ns), currentTimeInNano(0ns);

	frameTimer = currentTime.time_since_epoch();
	timer = currentTime.time_since_epoch();

	// Loop starts here
	while(running && !glfwWindowShouldClose(window)) {
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
			double start = glfwGetTime();
			render(); // Render in between physics updates
			double end = glfwGetTime();

			averageTimeToRenderFrame += (end - start) / glfwGetTime();

			frames++;
			frameTimer += nsPerRenderLimit;
		}

		// Do stuff every second
		if(currentTimeInNano - timer >= singleSecond) {
			tps = ticks;
			fps = frames;

			std::cout << "tps: " << tps << " fps: " << fps << std::endl;
			std::cout << "time to render = " << (averageTimeToRenderFrame * 1000.0) / frames << "ms" << std::endl;

			if(frameSampleIndex == 60) {
				double tenSecondAverage = 0.f;
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
