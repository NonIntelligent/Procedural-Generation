#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


class Terrain {
	const int MAP_SIZE;
	const unsigned int SEED;

	VertexTexture* vertices = nullptr;
	float** terrainMap = nullptr;
	unsigned int* terrainIndexData = nullptr;

	// Opengl stuff needed to render
	std::string shaderName;

	void buildIndexData();
	void buildNormalData();
	void buildTextureData();

public:
	VertexArray* va = nullptr;
	VertexBuffer* vb = nullptr;
	IndexBuffer* ib = nullptr;
	std::vector<ShaderUniform> uniforms;

	mat4 modelView = mat4(1.f);

	Terrain() : MAP_SIZE(33), SEED(123456789) {};
	Terrain(int mapSize, unsigned int seed);

	// Move constructor 
	Terrain(Terrain&& other) noexcept;
	Terrain& operator =(Terrain&& other) noexcept;

	~Terrain();
	
	void init(glm::vec4 initialHeight, glm::vec3 heightBounds, float randomRange, float roughness);

	void destroyTerrain();

	void setShaderName(std::string name);
	std::string getShaderName();
	int getMapSize();
	IndexBuffer* getIndexBuffer();
	VertexTexture* getVertices();
	unsigned int getSeed();

	void setClipPlane(glm::vec4 plane);
};

