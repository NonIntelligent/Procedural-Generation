#include "Generators/Water.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

#include <iostream>

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

Water::Water(int mapSize, float seaLevel) {
	if (mapSize <= 3) mapSize = 5;
	this->map_size = mapSize;
	this->seaLevel = seaLevel;

	vertices = new VertexBasic[mapSize * mapSize];

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
	if (map_size <= 0) return;

	int numStrips = map_size - 1;
	int verticesPerStrip = map_size * 2;

	// Initialise vertex array
	int i = 0;

	for (int z = 0; z < map_size; z++) {
		for (int x = 0; x < map_size; x++) {
			vertices[i].position = glm::vec3(x, terrainMap[x][z], z);
			i++;
		}
	}

	buildIndexData();

	// Generate vertex arrays and buffers
	va = new VertexArray();

	vb = new VertexBuffer(&vertices[0], map_size * map_size * sizeof(VertexBasic), map_size * map_size, GL_STATIC_DRAW);

	VertexBufferLayout layout;
	// Vertex data has 11 components
	layout.push<float>(3); // position
	va->addBuffer(*vb, layout);

	ib = new IndexBuffer(&terrainIndexData[0], (map_size - 1) * (map_size * 2) + numStrips, GL_STATIC_DRAW);

	reflectionB = new FrameBuffer(GL_COLOR_ATTACHMENT0);
	reflectionB->createTextureAttatchment(1280, 720);
	reflectionB->createDepthBuffer(1280, 720);
	reflectionB->checkStatus();

	refractionB = new FrameBuffer(GL_COLOR_ATTACHMENT0);
	refractionB->createTextureAttatchment(1280, 720);
	refractionB->createDepthTextureAttatchment(1280, 720);
	refractionB->checkStatus();
	reflectionB->unBind(1280, 720);

	mat4 modelView = mat4(1.f);
	modelView = translate(modelView, vec3(-map_size / 2.f, 0.f, -map_size / 2.f));
	modelView = scale(modelView, vec3(2.f, 1.5f, 2.f)); // Stretch the sea only in the x and z direction

	ShaderUniform modelMat;
	modelMat.name = "model";
	modelMat.dataMatrix = modelView;
	modelMat.type = UniformType::MAT4;

	ShaderUniform u_waveAmplitude;
	u_waveAmplitude.name = "waveAmplitude";
	u_waveAmplitude.dataMatrix[0][0] = 1.7f;
	u_waveAmplitude.type = UniformType::FLOAT1;

	ShaderUniform u_peakAmplitude;
	u_peakAmplitude.name = "peakAmplitude";
	u_peakAmplitude.dataMatrix[0][0] = 0.1f;
	u_peakAmplitude.type = UniformType::FLOAT1;

	ShaderUniform u_compression;
	u_compression.name = "compression";
	u_compression.dataMatrix[0][0] = 0.5f;
	u_compression.type = UniformType::FLOAT1;

	ShaderUniform u_waterTime;
	u_waterTime.name = "waveTime";
	u_waterTime.dataMatrix[0][0] = waveTime;
	u_waterTime.type = UniformType::FLOAT1;

	ShaderUniform u_reflectionTexture;
	u_reflectionTexture.name = "u_reflection";
	u_reflectionTexture.resourceName = "color_reflection";
	u_reflectionTexture.dataMatrix[0][0] = 1.f;
	u_reflectionTexture.type = UniformType::TEXTURE;

	ShaderUniform u_refractionTexture;
	u_refractionTexture.name = "u_refraction";
	u_refractionTexture.resourceName = "color_refraction";
	u_refractionTexture.dataMatrix[0][0] = 2.f;
	u_refractionTexture.type = UniformType::TEXTURE;

	ShaderUniform u_refractionDepthTexture;
	u_refractionDepthTexture.name = "u_depth";
	u_refractionDepthTexture.resourceName = "depth_refraction";
	u_refractionDepthTexture.dataMatrix[0][0] = 3.f;
	u_refractionDepthTexture.type = UniformType::TEXTURE;

	uniforms.push_back(modelMat);
	uniforms.push_back(u_peakAmplitude);
	uniforms.push_back(u_waveAmplitude);
	uniforms.push_back(u_compression);

	uniforms.push_back(u_reflectionTexture);
	uniforms.push_back(u_refractionTexture);
	uniforms.push_back(u_refractionDepthTexture);

	uniforms.push_back(u_waterTime);

	va->unBind();
	ib->unBind();
}

void Water::destroyWater() {
	// Object had used default constructor
	if (terrainMap == nullptr) return;

	for (int i = 0; i < map_size; i++) {
		delete[] terrainMap[i];
	}

	delete[] terrainMap;
	delete[] terrainIndexData;
	delete[] vertices;

	// init() was never called
	if (vb == nullptr) return;

	vb->unBind();
	delete(vb);
	ib->unBind();
	delete(ib);
	va->unBind();
	delete(va);
	if (reflectionB != nullptr) {
		reflectionB->unBind(1280, 720);
		delete(reflectionB);
		delete(refractionB);
	}

	uniforms.clear();
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

FrameBuffer* Water::getReflectionBuffer() {
	return reflectionB;
}

FrameBuffer* Water::getRefractionBuffer() {
	return refractionB;
}
