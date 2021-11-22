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
	grassCount = grassPoints.size();

	// Base vertex for a single grass blade
	vertices[0] = {glm::vec3(-0.2f, -0.2f, 0.5f)};
	vertices[1] = {glm::vec3(0.2f, -0.2f, 0.5f)};
	vertices[2] = {glm::vec3(-0.2f, 2.f, 0.5f)};
	vertices[3] = {glm::vec3(0.2f, 2.f, 0.5f)};
	vertices[4] = {glm::vec3(-0.2f, 4.f, 0.5f)};
	vertices[5] = {glm::vec3(0.2f, 4.f, 0.5f)};
	vertices[6] = {glm::vec3(0.f, 5.f, 0.5f)};

	// Setup triangle-strip indicies
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 1; indices[4] = 2; indices[5] = 3;
	indices[6] = 2; indices[7] = 3; indices[8] = 4;
	indices[9] = 3; indices[10] = 4; indices[11] = 5;
	indices[12] = 4; indices[13] = 5; indices[14] = 6;

}

void Grass::init(glm::mat4 terrainModelMatrix, unsigned int seed) {
	this->seed = seed;

	if (grassPoints.size() == 0) return;

	// Generate vertex arrays and buffers
	va = new VertexArray();

	vb = new VertexBuffer(vertices, sizeof(VertexBasic) * 7, 7, GL_STATIC_DRAW);

	VertexBufferLayout layout;
	layout.push<float>(3); // position
	va->addBuffer(*vb, layout);

	// Prepare instance data
	Generate::setRandomSeed(seed);
	instanceMatrices = new glm::mat4[grassCount]; // 1 instance per vertex provided by terrain. TODO add clusters

	float radius = 2.f; // Not in use yet
	float offset = 0.3f; // Not in use yet

	for (int i = 0; i < grassCount; i++) {
		glm::mat4 model = glm::mat4(1.f);
		
		glm::vec4 newPosition = glm::vec4(grassPoints[i].position, 1.0) * terrainModelMatrix;

		// create a grass at a given terrain vertex
		model = terrainModelMatrix;
		model = glm::translate(model, grassPoints[i].position);
		model = glm::scale(model, glm::vec3(1.f, 0.25f, 1.f));
		//model = glm::translate(model, glm::vec3(10.f, 10.f, Generate::random(50.f)));

		// Rotates a random amount around the unit-up axis
		float rotAngle = Generate::random(2 * 3.1415927f);
		model = glm::rotate(model, rotAngle, glm::vec3(0.f, 1.f, 0.f));

		instanceMatrices[i] = model;
	}

	// Add another vertex buffer to array for instancing
	vb2 = new VertexBuffer(instanceMatrices, grassCount * sizeof(glm::mat4), grassCount, GL_STATIC_DRAW);

	layout = VertexBufferLayout();
	layout.push<glm::mat4>(1, 1);
	va->addBuffer(*vb2, layout);

	ib = new IndexBuffer(indices, 15, GL_STATIC_DRAW);

	// Same scalar and translation as the Terrain class
	mat4 modelView = mat4(1.f);

	ShaderUniform modelMat;
	modelMat.name = "model";
	modelMat.dataMatrix = modelView;
	modelMat.type = UniformType::MAT4;

	ShaderUniform u_windTime;
	u_windTime.name = "windTime";
	u_windTime.dataMatrix[0][0] = windTime;
	u_windTime.type = UniformType::FLOAT1;

	ShaderUniform u_clipPlane;
	u_clipPlane.name = "plane";
	u_clipPlane.dataMatrix[0] = vec4(0.f, -1.f, 0.f, 1.0f);
	u_clipPlane.type = UniformType::VEC4;

	uniforms.push_back(modelMat);

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

