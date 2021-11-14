#include "Generators/Terrain.h"
#include "Generators/GenerateAlgorithms.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

#include <iostream>

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

void Terrain::buildTextureData() {
	float textureS = (float) MAP_SIZE * 0.04f;
	float textureT = (float) MAP_SIZE * 0.04f;
	int i = 0;
	float scaleC, scaleR;

	const float divisor = MAP_SIZE - 1.f;

	for (int x = 0; x < MAP_SIZE; x++) {
		for (int z = 0; z < MAP_SIZE; z++) {
			scaleC = x / divisor;
			scaleR = z / divisor;
			vertices[i].textcoord = {textureS * scaleC, textureT * scaleR};
			i++;
		}
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

		// illegal stuff right here
		int* ptr = (int*) this;
		*ptr = other.MAP_SIZE;
		ptr += 1;
		*ptr = other.SEED;

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

void Terrain::init(glm::vec4 initialHeight, glm::vec3 heightBounds, float randomRange, float roughness) {
	int numStrips = MAP_SIZE - 1;
	int verticesPerStrip = MAP_SIZE * 2;

	// Apply diamond square algorithm here
	Generate::setRandomSeed(SEED);

	std::cout << "Seed value: " << SEED << std::endl;

	// Initialise corners for algorithm
	terrainMap[0][0] = initialHeight.x;
	terrainMap[numStrips][0] = initialHeight.y;
	terrainMap[0][numStrips] = initialHeight.z;
	terrainMap[numStrips][numStrips] = initialHeight.w;

	Generate::DiamondSquare(terrainMap, MAP_SIZE, MAP_SIZE, randomRange, roughness);

	//////////////////////////////////////////////////////////


	// Initialise vertex array with values from terrain map
	int i = 0;

	for(int z = 0; z < MAP_SIZE; z++) {
		for(int x = 0; x < MAP_SIZE; x++) {
			vertices[i].position = glm::vec3(x, terrainMap[x][z], z);
			vertices[i].color = glm::vec3(0.f);
			vertices[i].normal = glm::vec3(0.f);
			vertices[i].textcoord = glm::vec2(0.f);
			i++;
		}
	}

	// Now build the index data 
	buildIndexData();

	// Generate normal data for each triangle face
	buildNormalData();

	// Generate uv coordinates for the terrain
	buildTextureData();

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
	modelView = translate(modelView, vec3(-MAP_SIZE / 2.f, 0.f, -MAP_SIZE / 2.f));
	modelView = scale(modelView, vec3(2.f, 4.f, 2.f));

	ShaderUniform modelViewMat;
	modelViewMat.name = "model";
	modelViewMat.dataMatrix = modelView;
	modelViewMat.type = UniformType::MAT4;

	ShaderUniform low;
	low.name = "lower";
	low.dataMatrix[0][0] = heightBounds.x;
	low.type = UniformType::FLOAT1;

	ShaderUniform high;
	high.name = "higher";
	high.dataMatrix[0][0] = heightBounds.y;
	high.type = UniformType::FLOAT1;

	ShaderUniform max;
	max.name = "maximum";
	max.dataMatrix[0][0] = heightBounds.z;
	max.type = UniformType::FLOAT1;

	ShaderUniform textureSlot1;
	textureSlot1.name = "u_texture1";
	textureSlot1.resourceName = "sand";
	textureSlot1.dataMatrix[0][0] = 1.f;
	textureSlot1.type = UniformType::TEXTURE;

	ShaderUniform textureSlot2;
	textureSlot2.name = "u_texture2";
	textureSlot2.resourceName = "grass";
	textureSlot2.dataMatrix[0][0] = 2.f;
	textureSlot2.type = UniformType::TEXTURE;

	ShaderUniform textureSlot3;
	textureSlot3.name = "u_texture3";
	textureSlot3.resourceName = "rock";
	textureSlot3.dataMatrix[0][0] = 3.f;
	textureSlot3.type = UniformType::TEXTURE;

	ShaderUniform textureSlot4;
	textureSlot4.name = "u_texture4";
	textureSlot4.resourceName = "snow";
	textureSlot4.dataMatrix[0][0] = 4.f;
	textureSlot4.type = UniformType::TEXTURE;

	ShaderUniform normalMatrix;
	normalMatrix.name = "normalMat";
	normalMatrix.dataMatrix = transpose(inverse(mat3(modelView)));
	normalMatrix.type = UniformType::MAT3;

	ShaderUniform u_clipPlane;
	u_clipPlane.name = "plane";
	u_clipPlane.dataMatrix[0] = vec4(0.f, -1.f, 0.f, 10.f);
	u_clipPlane.type = UniformType::VEC4;

	uniforms.push_back(modelViewMat);
	uniforms.push_back(low);
	uniforms.push_back(high);
	uniforms.push_back(max);
	uniforms.push_back(normalMatrix);
	uniforms.push_back(textureSlot1);
	uniforms.push_back(textureSlot2);
	uniforms.push_back(textureSlot3);
	uniforms.push_back(textureSlot4);

	uniforms.push_back(u_clipPlane);

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

void Terrain::setClipPlane(glm::vec4 plane) {
	int size = uniforms.size();

	uniforms[size - 1].dataMatrix[0] = plane;
}
