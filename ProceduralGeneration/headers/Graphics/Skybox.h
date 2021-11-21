#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

class Skybox { 
	// Opengl stuff needed to render
	std::string shaderName;

public:
	VertexArray* va = nullptr;
	VertexBuffer* vb = nullptr;
	std::vector<ShaderUniform> uniforms;

	glm::mat4 modelView;

	Skybox();
	~Skybox();

	void init();
	void destroySkybox();

	void setShaderName(std::string name);
	std::string getShaderName();
};

