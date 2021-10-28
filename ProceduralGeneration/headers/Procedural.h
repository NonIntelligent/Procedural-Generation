#pragma once
#include "macros/Macro_chrono.h"
#include "Scene.h"
#include "Graphics/Shader.h"
#include "Generators/Terrain.h"
#include "Graphics/Texture.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <vector>
#include <unordered_map>

const int CORE_COUNT = std::thread::hardware_concurrency();

using namespace glm;

class Procedural {
	GLFWwindow* window;

	// update rate of 30hz
	const nanoSeconds NS_PER_TICK = nanoSeconds(32222us);
	long targetFps = 8000;
	volatile bool running = false;
	const int threadCount = CORE_COUNT == 0 ? 12 : CORE_COUNT;
	double frameSamples[60] = {0.0};
	int frameSampleIndex = 0;
	std::vector<std::thread> threads;

	// Objects
	Scene scene;
	Terrain terrain = Terrain(5);
	std::unordered_map<std::string, Shader> shaders;
	std::vector<Texture> textures;

	// Camera options
	vec3 cameraPos;
	vec3 lookAtDir;
	float horizontalAngle, verticalAngle, g_fov;
	const float cameraSpeed = 2.f;
	mat4 projMat;

	bool initGLFW();
	void setupWindowHints();

	void updateKeyboardInput();
	void updateMouseInput();

public:
	Procedural();
	~Procedural();

	bool init();

	void createShaders();
	void createObjects();

	void handleInputs();
	void update();
	void render();

	Texture findTexture(const std::string& name);

	void updateShaderUniform(Shader* shader, const std::vector<ShaderUniform>& uniforms);

	void mainLoop();
};
