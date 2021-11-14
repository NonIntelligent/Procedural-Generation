#version 420 core

layout(location=0) in vec3 terrainCoords;
layout(location=1) in vec3 terrainColors;
layout(location=2) in vec3 terrainNormal;
layout(location=3) in vec2 terrainUV;

layout(std140, binding = 0) uniform camera
{
	mat4 proj;
	mat4 view;
	vec3 viewPos;
};

uniform mat4 model;
uniform mat3 normalMat;

flat out vec3 colorsExport;
out vec3 normalExport;
out vec2 uvExport;
out float heightExport;

out vec3 viewPosition;
out vec3 fragPos;

uniform vec4 plane;

void main(void)
{
	vec4 pos = vec4(terrainCoords, 1.0);

	gl_ClipDistance[0] = dot(pos, plane);

	gl_Position = proj * view * model * pos;
	colorsExport = terrainColors;
	normalExport = normalize(normalMat * terrainNormal);
	uvExport = terrainUV;
	heightExport = terrainCoords.y;

	viewPosition = viewPos;
	fragPos = vec3(model * vec4(terrainCoords, 1.f));
}