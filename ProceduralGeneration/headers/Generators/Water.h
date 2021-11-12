#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Water { 
	int map_size = 5;

	Vertex* vertices = nullptr;
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

	// Supply to shader simulate waves
	float waveTime = 0.0;
	float seaLevel = 0.f;
	
	Water() { };
	Water(int mapSize, float seaLevel);
	~Water() { };

	void init();

	void destroyWater();

	void setShaderName(std::string name);
	std::string getShaderName();
	int getMapSize();
	IndexBuffer* getIndexBuffer();
};

