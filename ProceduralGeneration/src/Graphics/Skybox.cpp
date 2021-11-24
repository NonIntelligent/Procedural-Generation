#include "Graphics/Skybox.h"

#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox() { 

}

Skybox::~Skybox() { }

void Skybox::init() {

	float cubeVertices[108] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	va = new VertexArray();

	vb = new VertexBuffer(cubeVertices, 36 * 3 * sizeof(float), 36, GL_STATIC_DRAW);

	VertexBufferLayout layout;
	layout.push<float>(3); // position
	va->addBuffer(*vb, layout);

	modelView = glm::mat4(1);
	modelView = glm::scale(modelView, vec3(1.f));

	ShaderUniform modelViewMat;
	modelViewMat.name = "model";
	modelViewMat.dataMatrix = modelView;
	modelViewMat.type = UniformType::MAT4;

	ShaderUniform u_skyboxTexture;
	u_skyboxTexture.name = "u_texture";
	u_skyboxTexture.resourceName = "skybox";
	u_skyboxTexture.dataMatrix[0][0] = 1.f;
	u_skyboxTexture.type = UniformType::CUBEMAP;

	uniforms.push_back(modelViewMat);
	uniforms.push_back(u_skyboxTexture);

}

void Skybox::destroySkybox() { 
	if (vb == nullptr) return;

	vb->unBind();
	delete(vb);
	va->unBind();
	delete(va);

	uniforms.clear();
}

void Skybox::setShaderName(std::string name) { }

std::string Skybox::getShaderName() {
	return shaderName;
}
