#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

class Grass {
	std::vector<Vertex> grassPoints;
	VertexUV vertices[7];
	unsigned int indices[15];

	int instanceCount = 0;
	int maxInstanceCount = 0;
	int clusterCount = 0;
	unsigned int seed = 0;

	// Opengl stuff needed to render
	std::string shaderName;
	glm::mat4* instanceMatrices;
	float* rands;

public:
	VertexArray* va = nullptr;
	VertexBuffer* vb = nullptr;
	VertexBuffer* vb2 = nullptr;
	VertexBuffer* vb3 = nullptr;
	IndexBuffer* ib = nullptr;
	std::vector<ShaderUniform> uniforms;

	float windTime = 0.f;

	Grass() { };
	Grass(Vertex* points, int width, float minHeight, float maxHeight);
	~Grass() { };

	void init(glm::mat4 terrainModelMatrix, int clusterCount, unsigned int seed);

	void cullGrass(glm::vec3 currentPosition, float distanceLimit, int instanceLimit);

	void destroyGrass();

	std::string getShaderName();
	int getVerticesCount();
	int getInstanceCount();

	void setShaderName(std::string name);
	void setClipPlane(glm::vec4 clipPlane);
};

