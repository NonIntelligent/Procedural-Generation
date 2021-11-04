#include "Procedural.h"
#include "Vendor/ErrorLogger.h"

#include <iostream>

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
	glfwSwapInterval(1);

	glfwSetKeyCallback(window, &Procedural::key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return true;
}

void Procedural::setupWindowHints() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
	
	vec3 temp(0.0);

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec3), &temp));

	temp.x = 1.f;
	temp.y = 1.f;
	temp.z = 1.f;

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(vec3), &temp));

	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(vec3), &temp));

	temp.x = 1.f;
	temp.y = 5.f;
	temp.z = 5.f;

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

Procedural::Procedural() {
	threads.reserve(threadCount);
}

Procedural::~Procedural() {
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

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xffffffff);

	perspectiveMat = perspective(radians(60.0), (double) width / height, 0.1, 100.0);
	lookAtCustom();

	setupGlobalUniforms();
	updateCameraUniform();

	createShaders();

	createObjects();

	return true;
}

void Procedural::createShaders() {
	Shader shader;
	//shader.parseShader("res/Shaders/vertexShader.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/vertexShader.glsl", GL_VERTEX_SHADER);
	shader.parseShader("res/Shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
	shader.createShader();

	shaders["terrain"] = shader;
}

void Procedural::createObjects(){
	terrain = Terrain(65, 123456789u);
	double start = glfwGetTime();
	terrain.init();
	double end = glfwGetTime();
	std::cout << "Time taken to generate Terrain: " << (end - start) << " seconds" << std::endl;
	terrain.setShaderName("terrain");
}

void Procedural::handleInputs() {
	glfwPollEvents();
	updateMouseInput();
}

void Procedural::update() {
	updateCamera();

	lookAtCustom();

	updateCameraUniform();
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

void Procedural::updateCamera() {
	
	cameraPos += xaxis * movementDir.x * cameraSpeed * 0.0333f;
	cameraPos += UP * movementDir.y * cameraSpeed * 0.0333f;
	cameraPos += zaxis * movementDir.z * cameraSpeed * 0.0333f;

	vec3 direction;

	// Convert screen space offset to clip space.
	double clipX = deltaX / width;
	double clipY = deltaY / height;

	horizontalAngle += toRadf(180.f * 1.f * clipX);
	verticalAngle += toRadf(180.f * 1.f * clipY);

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

	// Direction vector in world coordinates.
	direction.x = cosf(verticalAngle) * -sinf(horizontalAngle);
	direction.y = sinf(verticalAngle);
	direction.z = cosf(verticalAngle) * cosf(horizontalAngle);

	// Set new target position from origin.
	lookAtDir = direction;
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

void Procedural::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (action) {
	case GLFW_PRESS:
		movementDir.x = key == GLFW_KEY_A ? -1 : key == GLFW_KEY_D ? 1 : movementDir.x;
		movementDir.y = key == GLFW_KEY_LEFT_CONTROL ? -1 : key == GLFW_KEY_SPACE ? 1 : movementDir.y;
		movementDir.z = key == GLFW_KEY_W ? -1 : key == GLFW_KEY_S ? 1 : movementDir.z;

		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
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
		break;
	default:
		break;
	}
}
