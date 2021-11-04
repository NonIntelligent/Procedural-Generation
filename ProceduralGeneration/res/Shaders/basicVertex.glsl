#version 420 core

layout(location=0) in vec3 terrainCoords;
layout(location=1) in vec3 terrainColors;
layout(location=2) in vec3 terrainNormal;

layout(std140) uniform camera
{
	mat4 proj;
	mat4 view;
	vec3 viewPos;
};

uniform mat4 model;

flat out vec3 colorsExport;

void main(void)
{
	vec4 pos = vec4(terrainCoords, 1.0);
	gl_Position = proj * view * model * pos;
	colorsExport = terrainColors;
}