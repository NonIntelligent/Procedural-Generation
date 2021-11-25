#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

class Grass {
	std::vector<VertexBasic> grassPoints;
	VertexUV vertices[7];
	unsigned int indices[15];

	int instanceCount = 0;
	int maxInstanceCount = 0;
	int clusterCount = 0;
	unsigned int seed = 0;

	// Opengl stuff needed to render
	std::string shaderName;
	glm::mat4* instanceMatrices = nullptr;
	float* rands = nullptr;

public:
	VertexArray* va = nullptr;
	VertexBuffer* vb = nullptr;
	VertexBuffer* vb2 = nullptr;
	VertexBuffer* vb3 = nullptr;
	IndexBuffer* ib = nullptr;
	std::vector<ShaderUniform> uniforms;

	float windTime = 0.f;

	bool initialised = false;

	Grass() { };
	Grass(VertexTexture* points, int width, float minHeight, float maxHeight);
	~Grass() { };

	void init(glm::mat4 terrainModelMatrix, int clusterCount, unsigned int seed);

	/* Culls grass to a circular radius around the current position.
	* 
	*	Loops through all possible grass pointsand changes data in vertex buffer to only
	*	render the ones within the distanceLimit.
	*	@param instanceLimit Optionally limit the total number of instances to render (leave at 0 for no limit)
	*/
	void cullGrass(glm::vec3 currentPosition, float distanceLimit, int instanceLimit = 0);

	void destroyGrass();

	std::string getShaderName();
	int getVerticesCount();
	int getInstanceCount();

	void setShaderName(std::string name);
	void setClipPlane(glm::vec4 clipPlane);
};

