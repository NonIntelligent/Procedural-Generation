#include "Generators/Water.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

void Water::buildIndexData() {
	int i = 0;

	int jump = 0;
	int offset = 0;

	for (int z = 0; z < map_size - 1; z++) {
		i = z * map_size;

		jump = z * map_size * 2 + offset++;

		for (int x = 0; x < map_size * 2; x += 2) {
			terrainIndexData[jump + x] = i;
			i++;
		}
		for (int x = 1; x < map_size * 2 + 1; x += 2) {
			terrainIndexData[jump + x] = i;
			i++;
		}

		terrainIndexData[map_size * 2 * offset + offset - 1] = 0xffffffff;
	}
}

void Water::buildNormalData() {
	unsigned int index1, index2, index3, jump, offset = 0;
	float dot_value;
	vec3 pt1, pt2, pt3, ttVec, edge1, edge2, normal, up = {0.f,1.f,0.f};

	for (int x = 0; x < map_size - 1; x++) {
		jump = x * map_size * 2 + offset++;

		for (int z = 0; z < map_size * 2 - 2; z++) {
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
	int total = map_size * map_size;

	for (int i = 0; i < total - 1; i++) {
		vertices[i].normal = normalize(vertices[i].normal);
	}
}

void Water::buildTextureData() {
	float textureS = (float)map_size * 0.04f;
	float textureT = (float)map_size * 0.04f;
	int i = 0;
	float scaleC, scaleR;

	const float divisor = map_size - 1.f;

	for (int x = 0; x < map_size; x++) {
		for (int z = 0; z < map_size; z++) {
			scaleC = x / divisor;
			scaleR = z / divisor;
			vertices[i].textcoord = {textureS * scaleC, textureT * scaleR};
			i++;
		}
	}
}

Water::Water(int mapSize, float seaLevel) {
	this->map_size = mapSize;
	this->seaLevel = seaLevel;

	vertices = new Vertex[mapSize * mapSize];

	int numStripsRequired = mapSize - 1;
	int verticesPerStrip = mapSize * 2;

	terrainIndexData = new unsigned int[numStripsRequired * verticesPerStrip + numStripsRequired];

	// Initialise water - set values in the height map to the seaLevel
	terrainMap = new float* [map_size];
	for (int i = 0; i < map_size; i++) {
		terrainMap[i] = new float[map_size];
	}

	for (int x = 0; x < map_size; x++) {
		for (int z = 0; z < map_size; z++) {
			terrainMap[x][z] = seaLevel;
		}
	}
}

void Water::init() {
	int numStrips = map_size - 1;
	int verticesPerStrip = map_size * 2;

	// Initialise vertex array
	int i = 0;

	for (int z = 0; z < map_size; z++) {
		for (int x = 0; x < map_size; x++) {
			vertices[i].position = glm::vec3(x, terrainMap[x][z], z);
			vertices[i].color = glm::vec3(0.83f, 0.945f, 0.977f);
			vertices[i].normal = glm::vec3(0.f);
			vertices[i].textcoord = glm::vec2(0.f);
			i++;
		}
	}

	buildIndexData();

	buildNormalData();

	buildTextureData();

	// Generate vertex arrays and buffers
	va = new VertexArray();

	vb = new VertexBuffer(&vertices[0], map_size * map_size * sizeof(Vertex), map_size * map_size, GL_STATIC_DRAW);

	VertexBufferLayout layout;
	// Vertex data has 11 components
	layout.push<float>(3); // position
	layout.push<float>(3); // colour
	layout.push<float>(3); // normal
	layout.push<float>(2); // texture coords
	va->addBuffer(*vb, layout);

	ib = new IndexBuffer(&terrainIndexData[0], (map_size - 1) * (map_size * 2) + numStrips, GL_STATIC_DRAW);

	mat4 modelView = mat4(1.f);
	modelView = translate(modelView, vec3(-map_size / 2.f, 0.f, -map_size / 2.f));
	modelView = scale(modelView, vec3(4.f, 1.5f, 4.f)); // Stretch the sea only in the x and z direction

	ShaderUniform modelMat;
	modelMat.name = "model";
	modelMat.dataMatrix = modelView;
	modelMat.type = UniformType::MAT4;

	ShaderUniform normalMatrix;
	normalMatrix.name = "normalMat";
	normalMatrix.dataMatrix = transpose(inverse(mat3(modelView)));
	normalMatrix.type = UniformType::MAT3;

	ShaderUniform u_waveAmplitude;
	u_waveAmplitude.name = "waveAmplitude";
	u_waveAmplitude.dataMatrix[0][0] = 1.0f;
	u_waveAmplitude.type = UniformType::FLOAT1;

	ShaderUniform u_peakAmplitude;
	u_peakAmplitude.name = "peakAmplitude";
	u_peakAmplitude.dataMatrix[0][0] = 0.3f;
	u_peakAmplitude.type = UniformType::FLOAT1;

	ShaderUniform u_compression;
	u_compression.name = "compression";
	u_compression.dataMatrix[0][0] = 1.f;
	u_compression.type = UniformType::FLOAT1;

	ShaderUniform u_waterTime;
	u_waterTime.name = "waveTime";
	u_waterTime.dataMatrix[0][0] = waveTime;
	u_waterTime.type = UniformType::FLOAT1;

	uniforms.push_back(modelMat);
	uniforms.push_back(normalMatrix);
	uniforms.push_back(u_peakAmplitude);
	uniforms.push_back(u_waveAmplitude);
	uniforms.push_back(u_compression);


	uniforms.push_back(u_waterTime);

	va->unBind();
	ib->unBind();
}

void Water::destroyWater() {
	// Object had used default constructor
	if (terrainMap == nullptr) return;

	delete(terrainMap);
	delete(terrainIndexData);
	delete(vertices);

	// init() was never called
	if (vb == nullptr) return;

	vb->unBind();
	delete(vb);
	ib->unBind();
	delete(ib);
	va->unBind();
	delete(va);
}

void Water::setShaderName(std::string name) {
	this->shaderName = name;
}

std::string Water::getShaderName() {
	return shaderName;
}

int Water::getMapSize() {
	return map_size;
}

IndexBuffer* Water::getIndexBuffer() {
	return ib;
}
