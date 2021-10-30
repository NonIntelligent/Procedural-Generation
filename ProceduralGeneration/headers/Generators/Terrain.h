#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"


class Terrain {
	Vertex* vertices = nullptr;
	float** terrainMap = nullptr;
	unsigned int* terrainIndexData = nullptr;

	const int MAP_SIZE;
	const unsigned int SEED;

	// Opengl stuff needed to render
	std::string shaderName;

public:
	//unsigned int** terrainIndexData = nullptr;
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib = nullptr;
	std::vector<ShaderUniform> uniforms;

	Terrain() : MAP_SIZE(33), SEED(123456789) {};
	Terrain(int mapSize, unsigned int seed);
	~Terrain();
	
	void init();

	void destroyTerrain();

	void setShaderName(std::string name);
	std::string getShaderName();
	int getMapSize();
	IndexBuffer* getIndexBuffer();

};

