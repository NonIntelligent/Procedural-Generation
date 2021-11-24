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

uniform float waveAmplitude;
uniform float peakAmplitude;
uniform float compression;
uniform float waveTime;

flat out vec3 colorsExport;
out vec3 normalExport;
out vec2 uvExport;

out vec3 viewPosition;
out vec3 fragPos;
out vec4 clipSpace;

void main(void)
{
	vec4 pos = vec4(terrainCoords, 1.0);

	float firstDerivativeX = waveAmplitude * compression * cos(compression * (pos.x + waveTime)) 
	- 4.f * peakAmplitude * sin(4.f * pos.x + waveTime);

	float firstDerivativeZ = waveAmplitude * compression * cos(compression * (pos.z + waveTime)) 
	- 4.f * peakAmplitude * sin(4.f * pos.z + waveTime);

	float firstDerivative = firstDerivativeX + firstDerivativeZ;

	vec3 norm = vec3(-firstDerivative, 1.f, 0.f);

	gl_Position = proj * view * model * pos;
	clipSpace = gl_Position;
	colorsExport = terrainColors;
	normalExport = norm;
	uvExport = terrainUV;

	viewPosition = viewPos;
	fragPos = vec3(model * vec4(terrainCoords, 1.f));
}