#version 420 core

layout(location = 0) in vec3 position;

layout(std140, binding = 0) uniform camera
{
	mat4 proj;
	mat4 view;
	vec3 viewPos;
};

uniform mat4 model;

// Used to get rid of translation caused by view matrix.
mat4 viewStatic;

out vec3 texCoords;

void main() {
	texCoords = position;
	viewStatic = mat4(mat3(view));

	vec4 pos = proj * viewStatic * model * vec4(position, 1.0);
	// Trick to optimise number of pixels drawn but the skybox needs to be rendered last.
	gl_Position = pos.xyww;
};
