#include "Generators/Terrain.h"
#include "Generators/GenerateAlgorithms.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

void Terrain::buildIndexData() {
	int i = 0;

	int jump = 0;
	int offset = 0;

	for (int z = 0; z < MAP_SIZE - 1; z++) {
		i = z * MAP_SIZE;

		jump = z * MAP_SIZE * 2 + offset++;

		for (int x = 0; x < MAP_SIZE * 2; x += 2) {
			terrainIndexData[jump + x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2) {
			terrainIndexData[jump + x] = i;
			i++;
		}

		terrainIndexData[MAP_SIZE * 2 * offset + offset - 1] = 0xffffffff;
	}
}

void Terrain::buildNormalData() {
	unsigned int index1, index2, index3, jump, offset = 0;
	float dot_value;
	vec3 pt1, pt2, pt3, ttVec, edge1, edge2, normal, up = {0.f,1.f,0.f};

	for (int x = 0; x < MAP_SIZE - 1; x++) {
		jump = x * MAP_SIZE * 2 + offset++;

		for (int z = 0; z < MAP_SIZE * 2 - 2; z++) {
			index1 = terrainIndexData[z + jump];
			index2 = terrainIndexData[z + jump + 1];
			index3 = terrainIndexData[z + jump + 2];

			pt1 = vertices[index1].position;
			pt2 = vertices[index2].position;
			pt3 = vertices[index3].position;

			edge1 = pt2 - pt1;
			edge2 = pt3 - pt1;

			// Consistent normals upwards
			if (x % 2 == 1)
				ttVec = cross(edge2, edge1);
			else
				ttVec = cross(edge1, edge2);

			dot_value = dot(ttVec, up);
			if (dot_value < 0.000001f)
				normal = -ttVec;
			else
				normal = ttVec;

			// add normals to smooth later
			vertices[index1].normal = normal + vertices[index1].normal;
			vertices[index2].normal = normal + vertices[index2].normal;
			vertices[index3].normal = normal + vertices[index3].normal;
		}
	}

	// Smooth normals and normalize
	int total = MAP_SIZE * MAP_SIZE;

	for (int i = 0; i < total - 1; i++) {
		vertices[i].normal = normalize(vertices[i].normal);
	}

}

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

Terrain::Terrain(Terrain&& other) noexcept : MAP_SIZE(other.MAP_SIZE), SEED(other.SEED) {
	vertices = other.vertices;
	terrainIndexData = other.terrainIndexData;
	terrainMap = other.terrainMap;

	other.vertices = nullptr;
	other.terrainIndexData = nullptr;
	other.terrainMap = nullptr;
}

Terrain& Terrain::operator=(Terrain&& other) noexcept {
	if (this != &other) {
		// Delete current resources 
		destroyTerrain();

		vertices = other.vertices;
		terrainIndexData = other.terrainIndexData;
		terrainMap = other.terrainMap;

		other.vertices = nullptr;
		other.terrainIndexData = nullptr;
		other.terrainMap = nullptr;
	}

	return *this;
}

Terrain::~Terrain() {

}

void Terrain::init() {
	int numStrips = MAP_SIZE - 1;
	int verticesPerStrip = MAP_SIZE * 2;

	// Apply diamond square algorithm here
	Generate::setRandomSeed(SEED);

	float h1 = 1.f;
	float h2 = 1.f;
	float h3 = 1.f;
	float h4 = 1.f;

	// Initialise corners for algorithm
	terrainMap[0][0] = h1;
	terrainMap[numStrips][0] = h2;
	terrainMap[0][numStrips] = h4;
	terrainMap[numStrips][numStrips] = h3;

	Generate::DiamondSquare(terrainMap, MAP_SIZE, MAP_SIZE, 80.f, 1.1f);

	//////////////////////////////////////////////////////////


	// Initialise vertex array with values from terrain map
	int i = 0;

	for(int z = 0; z < MAP_SIZE; z++) {
		for(int x = 0; x < MAP_SIZE; x++) {
			vertices[i].position = glm::vec3(x, terrainMap[x][z], z);
			vertices[i].color = glm::vec3(0.f);
			vertices[i].normal = glm::vec3(0.f);
			i++;
		}
	}

	// Now build the index data 
	buildIndexData();

	// Generate normal data for each triangle face
	buildNormalData();

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
	modelView = translate(modelView, vec3(-MAP_SIZE / 2.f, -2.f, -MAP_SIZE / 2.f));

	ShaderUniform modelViewMat;
	modelViewMat.name = "model";
	modelViewMat.dataMatrix = modelView;
	modelViewMat.type = UniformType::MAT4;

	ShaderUniform normalMatrix;
	normalMatrix.name = "normalMat";
	normalMatrix.dataMatrix = transpose(inverse(mat3(modelView)));
	normalMatrix.type = UniformType::MAT3;

	uniforms.push_back(modelViewMat);
	uniforms.push_back(normalMatrix);

	va->unBind();
	ib->unBind();
}

void Terrain::destroyTerrain() {
	// Object had used default constructor
	if (terrainMap == nullptr) return;

	delete(terrainMap);
	delete(terrainIndexData);
	delete(vertices);

	// init was never called for terrain
	if (vb == nullptr) return;

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
