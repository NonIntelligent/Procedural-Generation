#include "Generators/Terrain.h"
#include "Generators/GenerateAlgorithms.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

Terrain::Terrain(int mapSize, unsigned int seed) : MAP_SIZE(mapSize), SEED(seed) {
	vertices = new Vertex[mapSize * mapSize];

	int numStripsRequired = mapSize - 1;
	int verticesPerStrip = mapSize * 2;

	terrainIndexData = new unsigned int[numStripsRequired * verticesPerStrip + numStripsRequired];

	// Initialise terrain - set values in the height map to 0
	terrainMap = new float* [MAP_SIZE];
	for(int i = 0; i < MAP_SIZE; i++) {
		terrainMap[i] = new float[MAP_SIZE];
	}

	for(int x = 0; x < MAP_SIZE; x++) {
		for(int z = 0; z < MAP_SIZE; z++) {
			terrainMap[x][z] = 0;
		}
	}
}

Terrain::~Terrain() {

}

void Terrain::init() {
	int numStrips = MAP_SIZE - 1;
	int verticesPerStrip = MAP_SIZE * 2;

	// Apply diamond square algorithm here
	Generate::setRandomSeed(SEED);
	float heightMultiplier = 3.f;

	float h1 = Generate::random();
	float h2 = Generate::random();
	float h3 = Generate::random();
	float h4 = Generate::random();

	// Initialise corners for algorithm
	terrainMap[0][0] = h1 * heightMultiplier;
	terrainMap[0][numStrips] = h2 * heightMultiplier;
	terrainMap[numStrips][0] = h3 * heightMultiplier;
	terrainMap[numStrips][numStrips] = h4 * heightMultiplier;

	Generate::DiamondSquare(terrainMap, MAP_SIZE, MAP_SIZE);

	//////////////////////////////////////////////////////////


	// Initialise vertex array with values from terrain map
	int i = 0;

	for(int z = 0; z < MAP_SIZE; z++) {
		for(int x = 0; x < MAP_SIZE; x++) {
			vertices[i].position = glm::vec3(x, terrainMap[x][z], z);
			vertices[i].color = glm::vec3(0.f);
			i++;
		}
	}


	// Now build the index data 
	i = 0;

	int jump;
	int offset = 0;

	for(int z = 0; z < numStrips; z++) {
		i = z * MAP_SIZE;

		jump = z * verticesPerStrip + offset++;

		for(int x = 0; x < MAP_SIZE * 2; x += 2) {
			terrainIndexData[jump + x] = i;
			i++;
		}
		for(int x = 1; x < MAP_SIZE * 2 + 1; x += 2) {
			terrainIndexData[jump + x] = i;
			i++;
		}

		terrainIndexData[verticesPerStrip * offset + offset - 1] = 65535;
	}

	// Generate vertex arrays and buffers
	va = new VertexArray();

	vb = new VertexBuffer(&vertices[0], MAP_SIZE * MAP_SIZE * sizeof(Vertex), MAP_SIZE * MAP_SIZE, GL_STATIC_DRAW);

	VertexBufferLayout layout;
	// Vertex data has 11 components
	layout.push<float>(3); // position
	layout.push<float>(3); // colour
	layout.push<float>(3); // normal
	layout.push<float>(2); // texture coords
	va->addBuffer(*vb, layout);

	ib = new IndexBuffer(&terrainIndexData[0], (MAP_SIZE - 1) * (MAP_SIZE * 2) + numStrips, GL_STATIC_DRAW);

	mat4 modelView = mat4(1.f);
	modelView = translate(modelView, vec3(-2.5f, -2.5f, -10.f));

	ShaderUniform modelViewMat;
	modelViewMat.name = "model";
	modelViewMat.dataMatrix = modelView;
	modelViewMat.type = UniformType::MAT4;

	uniforms.push_back(modelViewMat);

	va->unBind();
	ib->unBind();
}

void Terrain::destroyTerrain() {
	delete(terrainMap);
	delete(terrainIndexData);
	delete(vertices);

	vb->unBind();
	delete(vb);
	ib->unBind();
	delete(ib);
	va->unBind();
	delete(va);
}

void Terrain::setShaderName(std::string name) {
	shaderName = name;
}

std::string Terrain::getShaderName() {
	return shaderName;
}

int Terrain::getMapSize() {
	return MAP_SIZE;
}

IndexBuffer* Terrain::getIndexBuffer() {
	return ib;
}
