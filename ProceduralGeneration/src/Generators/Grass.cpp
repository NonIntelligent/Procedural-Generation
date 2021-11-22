#include "Generators/Grass.h"

#include "Generators/GenerateAlgorithms.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <vector>

Grass::Grass(Vertex* points, int maxSize, float minHeight, float maxHeight) { 
	for (int i = 0; i < maxSize; i++) {
		if (points[i].position.y > minHeight && points[i].position.y < maxHeight) {
			grassPoints.push_back(points[i]);
		}
	}

	grassPoints.shrink_to_fit();

	// Base vertex for a single grass blade
	vertices[0] = {glm::vec3(-0.1f, -0.2f, 0.f), glm::vec2(0.f, 0.f)};
	vertices[1] = {glm::vec3(0.1f, -0.2f, 0.f), glm::vec2(1.f, 0.f)};
	vertices[2] = {glm::vec3(-0.1f, 1.f, 0.f), glm::vec2(0.f, 0.33f)};
	vertices[3] = {glm::vec3(0.1f, 1.f, 0.f), glm::vec2(1.f, 0.33f)};
	vertices[4] = {glm::vec3(-0.1f, 3.f, 0.f), glm::vec2(0.f, 0.66f)};
	vertices[5] = {glm::vec3(0.1f, 3.f, 0.f), glm::vec2(1.f, 0.66f)};
	vertices[6] = {glm::vec3(-0.1f, 4.f, 0.f), glm::vec2(0.f, 1.f)};
	vertices[7] = {glm::vec3(0.1f, 4.f, 0.f), glm::vec2(1.f, 1.f)};

	// Setup triangle-strip indicies
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 1; indices[4] = 2; indices[5] = 3;
	indices[6] = 2; indices[7] = 3; indices[8] = 4;
	indices[9] = 3; indices[10] = 4; indices[11] = 5;
	indices[12] = 4; indices[13] = 5; indices[14] = 6;
	indices[15] = 5; indices[16] = 6; indices[17] = 7;

}

void Grass::init(glm::mat4 terrainModelMatrix, int clusterCount, unsigned int seed) {
	this->seed = seed;

	if (grassPoints.size() == 0) return;

	if (clusterCount <= 0) clusterCount = 1;

	grassCount = grassPoints.size() * clusterCount;

	// Generate vertex arrays and buffers
	va = new VertexArray();

	vb = new VertexBuffer(vertices, sizeof(VertexUV) * 8, 8, GL_STATIC_DRAW);

	VertexBufferLayout layout;
	layout.push<float>(3); // position
	layout.push<float>(2); // uv coordinates
	va->addBuffer(*vb, layout);

	// Prepare instance data
	Generate::setRandomSeed(seed);
	instanceMatrices = new glm::mat4[grassPoints.size() * clusterCount]; // 1 instance per vertex provided by terrain.
	float* rands = new float[grassPoints.size() * clusterCount];

	float radius = 2.f; // Not in use yet
	float offset = 0.3f; // Not in use yet

	int jump = 0;

	for (int i = 0; i < grassPoints.size(); i++) {
		jump = clusterCount * i;

		for (int c = 0; c < clusterCount; c++) {

			glm::mat4 model = glm::mat4(1.f);
		
			//glm::vec4 newPosition = glm::vec4(grassPoints[i].position, 1.0) * terrainModelMatrix;

			// create a grass at a given terrain vertex
			model = terrainModelMatrix;
			model = glm::translate(model, grassPoints[i].position);
			model = glm::scale(model, glm::vec3(1.f, 0.25f, 1.f));

			glm::vec3 clusterOffset(Generate::random(offset), Generate::random(offset), Generate::random(offset));

			model = glm::translate(model, clusterOffset);

			//model = glm::translate(model, glm::vec3(10.f, 10.f, Generate::random(50.f)));

			// Rotates a random amount around the unit-up axis
			float rotAngle = Generate::random(2 * 3.1415927f);
			model = glm::rotate(model, rotAngle, glm::vec3(0.f, 1.f, 0.f));

			instanceMatrices[jump + c] = model;

			rands[jump + c] = Generate::randomInRange(0.4f, 1.6f);
		}

	}

	// Add another vertex buffer to array for instancing
	vb2 = new VertexBuffer(instanceMatrices, grassCount * sizeof(glm::mat4), grassCount, GL_STATIC_DRAW);

	layout = VertexBufferLayout();
	layout.push<glm::mat4>(1, 1);
	va->addBuffer(*vb2, layout);

	VertexBuffer* vb3 = new VertexBuffer(rands, grassCount * sizeof(float), grassCount, GL_STATIC_DRAW);
	layout = VertexBufferLayout();
	layout.push<float>(1, 1);
	va->addBuffer(*vb3, layout);

	ib = new IndexBuffer(indices, 18, GL_STATIC_DRAW);

	// Same scalar and translation as the Terrain class
	mat4 modelView = mat4(1.f);

	ShaderUniform modelMat;
	modelMat.name = "model";
	modelMat.dataMatrix = modelView;
	modelMat.type = UniformType::MAT4;

	ShaderUniform textureSlot1;
	textureSlot1.name = "u_texture";
	textureSlot1.resourceName = "grassBlade";
	textureSlot1.dataMatrix[0][0] = 1.f;
	textureSlot1.type = UniformType::TEXTURE;

	ShaderUniform textureSlot2;
	textureSlot2.name = "u_texture_alpha";
	textureSlot2.resourceName = "grassBladeAlpha";
	textureSlot2.dataMatrix[0][0] = 2.f;
	textureSlot2.type = UniformType::TEXTURE;

	ShaderUniform u_windTime;
	u_windTime.name = "windTime";
	u_windTime.dataMatrix[0][0] = windTime;
	u_windTime.type = UniformType::FLOAT1;

	ShaderUniform u_clipPlane;
	u_clipPlane.name = "plane";
	u_clipPlane.dataMatrix[0] = vec4(0.f, -1.f, 0.f, 1.0f);
	u_clipPlane.type = UniformType::VEC4;

	uniforms.push_back(modelMat);
	uniforms.push_back(textureSlot1);
	uniforms.push_back(textureSlot2);

	uniforms.push_back(u_clipPlane);
	uniforms.push_back(u_windTime);

	va->unBind();
	ib->unBind();
}

void Grass::destroyGrass() { 
	// Object had used default constructor
	if (grassPoints.size() > 0) return;

	grassPoints.clear();

	// init() was never called
	if (vb == nullptr) return;

	delete(instanceMatrices);

	vb->unBind();
	delete(vb);
	vb2->unBind();
	delete(vb2);
	va->unBind();
	delete(va);
	ib->unBind();
	delete(ib);
}

std::string Grass::getShaderName() {
	return shaderName;
}

void Grass::setClipPlane(glm::vec4 plane) {
	int size = uniforms.size();

	uniforms[size - 2].dataMatrix[0] = plane;
}

int Grass::getVerticesCount() {
	return grassCount;
}

void Grass::setShaderName(std::string name) {
	shaderName = name;
}

