#pragma once
#include "macros/Macro_chrono.h"
#include "Graphics/Shader.h"
#include "Generators/Terrain.h"
#include "Generators/Water.h"
#include "Graphics/Texture.h"
#include "macros/Definitions.h"

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

using namespace glm;

class Procedural {
	GLFWwindow* window;

	// update rate of 30hz
	const nanoSeconds NS_PER_TICK = nanoSeconds(32222us);
	long targetFps = 8000;
	volatile bool running = false;
	double frameSamples[60] = {0.0};
	int frameSampleIndex = 0;
	int keys[GLFW_KEY_LAST] = {0};

	// Objects
	Terrain terrain;
	Water sea;
	std::unordered_map<std::string, Shader> shaders;
	std::vector<Texture> textures;

	// Camera options
	GLuint u_cameraID;
	vec3 cameraPos = {0.f, 0.f, 0.f};
	vec3 lookAtDir = {0.f, 0.f, -1.f};
	const vec3 UP = {0.f, 1.f, 0.f};
	vec3 xaxis;
	vec3 yaxis;
	vec3 zaxis;
	float horizontalAngle = PI_f, verticalAngle = 0.f;
	static float cameraSpeed;
	mat4 perspectiveMat = mat4(0.f);
	mat4 lookAtMat = mat4(0.f);

	// Lighting
	GLuint u_lightID;

	float width = 1280, height = 720;

	bool initGLFW();
	void setupWindowHints();
	void setupGlobalUniforms();

	void updateCameraUniform();
	void updateKeyboardInput();
	void updateMouseInput();

	void lookAtCustom();

	void calcLookAtDir();

public:
	Procedural();
	~Procedural();

	bool init();

	void createShaders();
	void createObjects();

	void handleInputs();
	void update();
	void render();

	void renderTerrain();
	void renderTrees();
	void renderGrass();
	void renderWater();

	Texture findTexture(const std::string& name);

	void updateCamera();
	void updateShaderUniform(Shader* shader, const std::vector<ShaderUniform>& uniforms);

	void mainLoop();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
