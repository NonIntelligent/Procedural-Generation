#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

class Grass {
	std::vector<Vertex> grassPoints;
	VertexBasic vertices[7];
	unsigned int indices[15];

	glm::mat4* instanceMatrices = nullptr;

	int grassCount = 0;
	unsigned int seed = 0;

	// Opengl stuff needed to render
	std::string shaderName;

public:
	VertexArray* va = nullptr;
	VertexBuffer* vb = nullptr;
	VertexBuffer* vb2 = nullptr;
	IndexBuffer* ib = nullptr;
	std::vector<ShaderUniform> uniforms;

	float windTime = 0.f;

	Grass() { };
	Grass(Vertex* points, int maxSize, float minHeight, float maxHeight);
	~Grass() { };

	void init(glm::mat4 terrainModelMatrix, unsigned int seed);

	void destroyGrass();

	std::string getShaderName();
	int getVerticesCount();

	void setShaderName(std::string name);
	void setClipPlane(glm::vec4 clipPlane);
};

