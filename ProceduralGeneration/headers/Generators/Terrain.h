#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"


class Terrain {
	Vertex* vertices = nullptr;
	float** terrainMap = nullptr;
	unsigned int* terrainIndexData = nullptr;

	const int MAP_SIZE;

	// Opengl stuff needed to render
	std::string shaderName;

public:
	//unsigned int** terrainIndexData = nullptr;
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib = nullptr;
	std::vector<ShaderUniform> uniforms;

	Terrain() : MAP_SIZE(33) {};
	Terrain(int mapSize);
	~Terrain();
	
	void init();

	void destroyTerrain();

	void setShaderName(std::string name);
	std::string getShaderName();
	int getMapSize();
	IndexBuffer* getIndexBuffer();

};

