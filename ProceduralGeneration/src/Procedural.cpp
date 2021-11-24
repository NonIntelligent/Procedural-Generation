#include "Procedural.h"
#include "Vendor/ErrorLogger.h"

#include <iostream>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

float Procedural::cameraSpeed = 20.f;

// Error callback for GLFW
void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

bool Procedural::initGLFW() {
	glfwSetErrorCallback(&error_callback);

	glfwTerminate();

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
	glfwSwapInterval(0);

	glfwSetKeyCallback(window, &Procedural::key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return true;
}

void Procedural::setupWindowHints() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_DEPTH_BITS, 32);
}

void Procedural::setupGlobalUniforms() {
	GLCall(glGenBuffers(1, &u_cameraID));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, u_cameraID));
	// 2 mat4 matrices and a vec3
	GLCall(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4) + sizeof(vec3), NULL, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	// Bind uniform buffer to a binding point 0, with the buffer object at offset 0 of size.
	GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, u_cameraID, 0, 2 * sizeof(mat4) + sizeof(vec3)));

	updateCameraUniform();

	GLCall(glGenBuffers(1, &u_lightID));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, u_lightID));

	// 4 vec3 + 12 bytes of padding as layout is in multiples of 16 bytes

	GLCall(glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(vec3) + 12, NULL, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	// Bind uniform buffer to a binding point 1, with the buffer object at offset 0 of size.
	GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 1, u_lightID, 0, 4 * sizeof(vec3) + 12));

	// Update light uniform
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, u_lightID));
	
	vec3 temp(0.1f);

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec3), &temp));

	temp.x = 0.5f;
	temp.y = 0.5f;
	temp.z = 0.5f;

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(vec3), &temp));

	temp.x = 0.7f;
	temp.y = 0.7f;
	temp.z = 0.7f;

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(vec3), &temp));

	temp.x = 350.f;
	temp.y = 250.f;
	temp.z = 900.f;

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 48, sizeof(vec3), &temp));

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void Procedural::updateCameraUniform() {
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, u_cameraID));

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), (float*) (&perspectiveMat[0]) ));

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), (float*)(&lookAtMat[0]) ));

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), sizeof(vec3), (float*)(&cameraPos[0])));

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

// used for camera movement
vec3 movementDir = vec3(0);
double mouseX = 0, mouseY = 0;
double lastMouseX = 0, lastMouseY = 0;
double deltaX = 0, deltaY = 0;
bool stats = false;

void Procedural::updateKeyboardInput() {
	glfwPollEvents();
}

void Procedural::updateMouseInput() {
	static bool firstMouse = true;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (firstMouse) {
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		firstMouse = false;
	}

	// Calc offsets. Y is inverted due to different coordinate spaces.

	deltaX = mouseX - lastMouseX;
	deltaY = lastMouseY - mouseY;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

void Procedural::lookAtCustom() {
	vec3 xAxis;
	vec3 zAxis;
	vec3 yAxis;
	vec3 dotPos;

	zAxis = (cameraPos + lookAtDir) - cameraPos;
	normalize(zAxis);

	xAxis = normalize(cross(zAxis, UP));

	yAxis = cross(xAxis, zAxis);

	zAxis *= -1.f;

	// Set global variables so that I can move along the directional axis
	xaxis = xAxis;
	yaxis = yAxis;
	zaxis = zAxis;

	dotPos.x = -dot(xAxis, cameraPos);
	dotPos.y = -dot(yAxis, cameraPos);
	dotPos.z = -dot(zAxis, cameraPos);

	lookAtMat[0] = vec4(xAxis, 0);
	lookAtMat[1] = vec4(yAxis, 0);
	lookAtMat[2] = vec4(zAxis, 0);
	lookAtMat[3] = vec4(0);
	lookAtMat = transpose(lookAtMat);
	lookAtMat[3] = vec4(dotPos, 1);
}

void Procedural::calcLookAtDir() {
	vec3 direction;

	// Direction vector in world coordinates.
	direction.x = cosf(verticalAngle) * -sinf(horizontalAngle);
	direction.y = sinf(verticalAngle);
	direction.z = cosf(verticalAngle) * cosf(horizontalAngle);

	// Set new target position from origin.
	lookAtDir = direction;
}

Procedural::Procedural() {

}

Procedural::~Procedural() {
	for (auto& elem : shaders) {
		elem.second.deleteProgram();
	}
	shaders.clear();

	for (auto& texture : textures) {
		texture.deleteTexture();
	}
	textures.clear();

	// destory shaders, buffers linked to the generation
	terrain.destroyTerrain();
	sea.destroyWater();
	grass.destroyGrass();
	skybox.destroySkybox();

	// Delete global uniforms
	GLCall(glDeleteBuffers(1, &u_cameraID));
	GLCall(glDeleteBuffers(1, &u_lightID));

	glfwTerminate();
}

bool Procedural::init() {
	bool glfw = initGLFW();
	GLenum error;

	std::cout << "starting memory ";
	int start = printMemoryUsage();

	if((error = glewInit()) != GLEW_OK) {
		std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
		glfwTerminate();
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CLIP_DISTANCE0);

	//glEnable(GL_CULL_FACE);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xffffffff);

	perspectiveMat = perspective(radians(60.0), (double) width / height, 0.1, 1000.0);
	lookAtCustom();

	setupGlobalUniforms();
	updateCameraUniform();

	createShaders();

	// Load all textures in use
	Texture texture = Texture();
	texture.loadTexture("res/grass.jpg", "grass");
	textures.push_back(texture);

	texture = Texture();
	texture.loadTexture("res/rock.jpg", "rock");
	textures.push_back(texture);

	texture = Texture();
	texture.loadTexture("res/sand.jpg", "sand");
	textures.push_back(texture);

	texture = Texture();
	texture.loadTexture("res/snow.png", "snow");
	textures.push_back(texture);

	texture = Texture();
	texture.loadTexture("res/grassGreen.jpg", "grassBlade");
	textures.push_back(texture);

	std::vector<std::string> texturePaths;

	texturePaths.push_back("res/skyboxNightRIGHT.png");
	texturePaths.push_back("res/skyboxNightLEFT.png");
	texturePaths.push_back("res/skyboxNightBOTTOM.png");
	texturePaths.push_back("res/skyboxNightTOP.png");
	texturePaths.push_back("res/skyboxNightFRONT.png");
	texturePaths.push_back("res/skyboxNightBACK.png");

	texture = Texture();
	if (!texture.loadCubeMap(texturePaths.data(), "skybox")) {
		std::cout << "Cubemap failed to load entirely." << std::endl;
	}
	textures.push_back(texture);

	createObjects();

	// Textures for fbo in water
	texture = Texture();
	auto buffer = sea.getReflectionBuffer();
	texture.setTextureID(buffer->getColorTexture());
	texture.setResourceName("color_reflection");
	textures.push_back(texture);

	texture = Texture();
	buffer = sea.getRefractionBuffer();
	texture.setTextureID(buffer->getColorTexture());
	texture.setResourceName("color_refraction");
	textures.push_back(texture);

	texture = Texture();
	texture.setTextureID(buffer->getDepthTexture());
	texture.setResourceName("depth_refraction");
	textures.push_back(texture);

	std::cout << "memory after init";
	int end = printMemoryUsage();

	std::cout << "Memory in use: " << end - start << "MB"<< std::endl;

	return true;
}

void Procedural::createShaders() {
	Shader shader;
	shader.parseShader("res/Shaders/vertexShader.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/TerrainFragment.glsl", GL_FRAGMENT_SHADER);
	shader.createShader();

	shaders["terrain"] = shader;

	shader = Shader();
	shader.parseShader("res/Shaders/waterVertex.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/waterFragment.glsl", GL_FRAGMENT_SHADER);
	shader.createShader();

	shaders["sea"] = shader;
	
	shader = Shader();
	shader.parseShader("res/Shaders/grassVertex.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/grassFragment.glsl", GL_FRAGMENT_SHADER);
	shader.createShader();

	shaders["grass"] = shader;

	shader = Shader();
	shader.parseShader("res/Shaders/skyboxVertex.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/skyboxFragment.glsl", GL_FRAGMENT_SHADER);
	shader.createShader();

	shaders["skybox"] = shader;
}

void Procedural::createObjects(){
	terrain = Terrain(513, 847866344);
	double start = glfwGetTime();
	terrain.init(vec4(25.f), vec3(4.f, 16.f, 32.f), 45.f, 1.1f);
	double end = glfwGetTime();
	std::cout << "Time taken to generate Terrain: " << (end - start) << " seconds" << std::endl;
	terrain.setShaderName("terrain");

	sea = Water(1025, 0.f);
	start = glfwGetTime();
	sea.init();
	end = glfwGetTime();
	std::cout << "Time taken to generate Sea: " << (end - start) << " seconds" << std::endl;

	sea.setShaderName("sea");

	grass = Grass(terrain.getVertices(), terrain.getMapSize(), 4.f, 16.f);
	start = glfwGetTime();
	grass.init(terrain.modelView, 6, terrain.getSeed());
	end = glfwGetTime();
	std::cout << "Time taken to generate Grass: " << (end - start) << " seconds" << std::endl;

	grass.setShaderName("grass");

	skybox = Skybox();
	skybox.init();
	skybox.setShaderName("skybox");

	std::cout << "Total number of Terrain vertices generated: " << terrain.getMapSize() * terrain.getMapSize() << std::endl;
	std::cout << "Total number of Water vertices generated: " << sea.getMapSize() * sea.getMapSize() << std::endl;
	std::cout << "Total number of Grass vertices generated: " << grass.getVerticesCount() << std::endl;
}

void Procedural::handleInputs() {
	glfwPollEvents();
	
}

void Procedural::update() {
	updateMouseInput();
	updateCamera();

	// Lock camera position for benchmarking
	cameraPos.x = 434.5f;
	cameraPos.y = 162.0f;
	cameraPos.z = -3.05f;

	lookAtCustom();
}

void Procedural::render() {
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render to other frame buffers i.e. water reflection
	sea.getReflectionBuffer()->bind(1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set clipping planes to render everything above sea level
	terrain.setClipPlane({0.f, 1.f, 0.f, -sea.seaLevel});
	grass.setClipPlane({0.f, 1.f, 0.f, -sea.seaLevel});
	// Move camera for reflection render
	float distance = 2 * cameraPos.y - sea.seaLevel;

	cameraPos.y -= distance;
	verticalAngle *= -1.f;
	calcLookAtDir();
	// Lock camera direction for more reliable benchmark comparisons
	lookAtDir = glm::vec3(-0.35f, -0.42f, 0.83f);
	lookAtCustom();
	updateCameraUniform();

	renderSkybox();
	renderTerrain();
	renderGrass();

	// Return camera to original position
	cameraPos.y += distance;
	verticalAngle *= -1.f;
	calcLookAtDir();
	lookAtDir = glm::vec3(-0.35f, -0.42f, 0.83f);
	lookAtCustom();
	updateCameraUniform();

	sea.getRefractionBuffer()->bind(1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	terrain.setClipPlane({0.f, -1.f, 0.f, sea.seaLevel});
	grass.setClipPlane({0.f, -1.f, 0.f, sea.seaLevel});
	renderSkybox();
	renderTerrain();
	renderGrass();

	// Render to main frame buffer
	sea.getReflectionBuffer()->unBind(1280, 720); // unbind to use main buffer
	terrain.setClipPlane({0.f, 0.f, 0.f, 0.f});
	grass.setClipPlane({0.f, 0.f, 0.f, 0.f});
	renderSkybox();
	renderTerrain();
	renderGrass();

	renderWater();

	glfwSwapBuffers(window);
}

void Procedural::renderTerrain() {
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
}

void Procedural::renderTrees() {

}

void Procedural::renderGrass() {
	Shader* current = nullptr;

	auto result = shaders.find("grass");
	if (result != shaders.end()) {
		current = &(result->second);
	}

	current->bind();
	grass.va->bind();

	grass.uniforms.back().dataMatrix[0][0] = glfwGetTime();
	updateShaderUniform(current, grass.uniforms);

	grass.ib->bind();

	GLCall(glDrawElementsInstanced(GL_TRIANGLE_STRIP, grass.ib->getCount(), GL_UNSIGNED_INT, nullptr, grass.getVerticesCount()));
}

void Procedural::renderWater() {
	Shader* current = nullptr;

	auto result = shaders.find("sea");
	if (result != shaders.end()) {
		current = &(result->second);
	}

	current->bind();
	sea.va->bind();

	sea.uniforms.back().dataMatrix[0][0] = glfwGetTime();
	updateShaderUniform(current, sea.uniforms);

	sea.ib->bind();

	GLCall(glDrawElements(GL_TRIANGLE_STRIP, sea.ib->getCount(), GL_UNSIGNED_INT, nullptr));
}

void Procedural::renderSkybox() {
	glDepthMask(GL_FALSE);
	Shader* current = nullptr;

	auto result = shaders.find("skybox");
	if (result != shaders.end()) {
		current = &(result->second);
	}

	current->bind();
	skybox.va->bind();
	updateShaderUniform(current, skybox.uniforms);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	glDepthMask(GL_TRUE);
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

int Procedural::printMemoryUsage() {
	GLint total_mem_kb = 0;
	GLCall(glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
				  &total_mem_kb));

	GLint cur_avail_mem_kb = 0;
	GLCall(glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
				  &cur_avail_mem_kb));

	std::cout << "VRAM Usage: " << (total_mem_kb - cur_avail_mem_kb) / 1000 << "MB / " << total_mem_kb / 1000<< "MB" << std::endl;

	return (total_mem_kb - cur_avail_mem_kb) / 1000;
}

void Procedural::updateCamera() {
	
	cameraPos += xaxis * movementDir.x * cameraSpeed * 0.0333f;
	cameraPos += UP * movementDir.y * cameraSpeed * 0.0333f;
	cameraPos += zaxis * movementDir.z * cameraSpeed * 0.0333f;

	// Convert screen space offset to clip space.
	double clipX = deltaX / width;
	double clipY = deltaY / height;

	horizontalAngle += toRadf(180.f * 0.5f * clipX);
	verticalAngle += toRadf(180.f * 0.5f * clipY);

	// Keeps angles within range of -2PI -> +2PI
	// Absolutely no idea if this is better than 2 conditional branches/statements.
	if (fabsf(horizontalAngle) >= 2 * PI_f) {
		horizontalAngle = fmodf(horizontalAngle, PI_f);
	}

	// Limit how far you can look up/down to +85 and -75 degrees.
	if (verticalAngle > 1.483f) {
		verticalAngle = 1.483f;
	}
	else if (verticalAngle < -1.309f) {
		verticalAngle = -1.309f;
	}

	calcLookAtDir();
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
		case UniformType::MAT3:
			shader->setUniformMatrix3fv((*it).name, 1, GL_FALSE, (*it).dataMatrix);
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

			unsigned int bindingPoint = (unsigned int) (*it).dataMatrix[0][0];
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

void Procedural::updatePerspective(double fov, double near, double far) {
	perspectiveMat = perspective(radians(fov), (double)width / height, near, far);

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, u_cameraID));

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), (float*)(&perspectiveMat[0])));

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
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

			averageTimeToRenderFrame += (end - start);

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

				std::cout << "average time to render last 60 seconds = " << (tenSecondAverage * 1000.f) / frames << "ms" << std::endl;

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

bool wireframeMode = false;

void Procedural::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (action) {
	case GLFW_PRESS:
		movementDir.x = key == GLFW_KEY_A ? -1 : key == GLFW_KEY_D ? 1 : movementDir.x;
		movementDir.y = key == GLFW_KEY_LEFT_CONTROL ? -1 : key == GLFW_KEY_SPACE ? 1 : movementDir.y;
		movementDir.z = key == GLFW_KEY_W ? -1 : key == GLFW_KEY_S ? 1 : movementDir.z;

		if (key == GLFW_KEY_LEFT_SHIFT) {
			cameraSpeed = 60.f;
		}

		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		if (key == GLFW_KEY_F) {
			if (!wireframeMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			wireframeMode = !wireframeMode;
		}

		break;

	case GLFW_RELEASE:
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
			movementDir.x = 0;
		}
		if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_SPACE) {
			movementDir.y = 0;
		}
		if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
			movementDir.z = 0;
		}

		if (key == GLFW_KEY_LEFT_SHIFT) {
			cameraSpeed = 20.f;
		}
		break;
	default:
		break;
	}
}
