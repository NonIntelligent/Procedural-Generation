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

	int grassCount = 0;
	unsigned int seed = 0;

	// Opengl stuff needed to render
	std::string shaderName;

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

	void destroyGrass();

	std::string getShaderName();
	int getVerticesCount();

	void setShaderName(std::string name);
	void setClipPlane(glm::vec4 clipPlane);
};

