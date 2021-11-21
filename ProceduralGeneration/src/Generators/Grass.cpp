#include "Generators/Grass.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <vector>

Grass::Grass(Vertex* points, int maxSize, float minHeight, float maxHeight) { 
	for (int i = 0; i < maxSize; i++) {
		if (points[i].position.y > minHeight && points[i].position.y < maxHeight) {
			vertices.push_back(points[i]);
		}
	}

	vertices.shrink_to_fit();
	grassCount = vertices.size();
}

void Grass::init(glm::mat4 terrainModelMatrix) {

	if (vertices.size() == 0) return;

	// Generate vertex arrays and buffers
	va = new VertexArray();

	vb = new VertexBuffer(&vertices[0], sizeof(Vertex) * vertices.size(), vertices.size(), GL_STATIC_DRAW);

	VertexBufferLayout layout;
	// Vertex data has 11 components
	layout.push<float>(3); // position
	layout.push<float>(3); // colour
	layout.push<float>(3); // normal
	layout.push<float>(2); // texture coords
	va->addBuffer(*vb, layout);

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

	uniforms.push_back(modelMat);

	ShaderUniform u_clipPlane;
	u_clipPlane.name = "plane";
	u_clipPlane.dataMatrix[0] = vec4(0.f, -1.f, 0.f, 1.0f);
	u_clipPlane.type = UniformType::VEC4;

	uniforms.push_back(u_clipPlane);
	uniforms.push_back(u_windTime);
}

void Grass::destroyGrass() { 
	// Object had used default constructor
	if (vertices.size() > 0) return;

	vertices.clear();

	// init() was never called
	if (vb == nullptr) return;

	vb->unBind();
	delete(vb);
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

