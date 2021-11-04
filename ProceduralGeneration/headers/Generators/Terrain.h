#pragma once
#include "Graphics/VertexObjects.h"
#include "Graphics/Shader.h"


class Terrain {
	const int MAP_SIZE;
	const unsigned int SEED;

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

	Terrain() : MAP_SIZE(33), SEED(123456789) {};
	Terrain(int mapSize, unsigned int seed);

	// Move constructor 
	Terrain(Terrain&& other) noexcept;
	Terrain& operator =(Terrain&& other) noexcept;

	~Terrain();
	
	void init();


	void destroyTerrain();

	void setShaderName(std::string name);
	std::string getShaderName();
	int getMapSize();
	IndexBuffer* getIndexBuffer();

};

