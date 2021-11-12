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

uniform float waveAmplitude;
uniform float peakAmplitude;
uniform float compression;
uniform float waveTime;

flat out vec3 colorsExport;
out vec3 normalExport;
out vec2 uvExport;
out float heightExport;

out vec3 viewPosition;
out vec3 fragPos;

void main(void)
{
	vec4 pos = vec4(terrainCoords, 1.0);

	pos.y += waveAmplitude * (sin(compression * (pos.x + waveTime)) + peakAmplitude * cos(4.f * pos.z + waveTime));

	float normalGradient = -16.f * peakAmplitude * cos(4.f * pos.z + waveTime) 
	- waveAmplitude * compression * compression * sin(compression * (pos.x + waveTime));

	float secondDerivative = waveAmplitude * compression * cos(compression * (pos.x + waveTime)) 
	- 4.f * peakAmplitude * sin(4.f * pos.z + waveTime);

	bool bothPositive = secondDerivative > 0.f && normalGradient > 0.f;
	bool bothNegative = secondDerivative < 0.f && normalGradient < 0.f;

	if (secondDerivative == 0.f) normalGradient = 0.f;
	else if (bothPositive || bothNegative) normalGradient *= -1.f;

	vec3 norm;

	if (normalGradient != 0.f) {
		float val1 = normalGradient * pos.x;
		float val2 = normalGradient * (pos.x + 1.f);
		vec3 norm = vec3(1.f, val2 - val1, 0.f);
	}
	else {
		norm = vec3(0.f, 1.f, 0.f);
	}



	gl_Position = proj * view * model * pos;
	colorsExport = terrainColors;
	normalExport = norm;
	uvExport = terrainUV;
	heightExport = terrainCoords.y;

	viewPosition = viewPos;
	fragPos = vec3(model * vec4(terrainCoords, 1.f));
}