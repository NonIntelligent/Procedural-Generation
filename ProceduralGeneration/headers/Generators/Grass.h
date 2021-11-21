#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

class Grass {
	std::vector<Vertex> vertices;

	int grassCount = 0;

	// Opengl stuff needed to render
	std::string shaderName;

public:
	VertexArray* va = nullptr;
	VertexBuffer* vb = nullptr;
	std::vector<ShaderUniform> uniforms;

	float windTime = 0.f;

	Grass() { };
	Grass(Vertex* points, int maxSize, float minHeight, float maxHeight);
	~Grass() { };

	void init(glm::mat4 terrainModelMatrix);

	void destroyGrass();

	std::string getShaderName();
	int getVerticesCount();

	void setShaderName(std::string name);
	void setClipPlane(glm::vec4 clipPlane);
};

