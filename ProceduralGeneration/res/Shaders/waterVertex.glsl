#version 420 core

layout(location=0) in vec3 waterCoords;

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

out vec3 normalExport;

out vec3 viewPosition;
out vec3 fragPos;
out vec4 clipSpace;

void main(void)
{
	vec4 pos = vec4(waterCoords, 1.0);

	float firstDerivativeX = waveAmplitude * compression * cos(compression * (pos.x + waveTime)) 
	- 4.f * peakAmplitude * sin(4.f * pos.x + waveTime);

	float firstDerivativeZ = waveAmplitude * compression * cos(compression * (pos.z + waveTime)) 
	- 4.f * peakAmplitude * sin(4.f * pos.z + waveTime);

	float firstDerivative = firstDerivativeX + firstDerivativeZ;

	gl_Position = proj * view * model * pos;
	clipSpace = gl_Position;
	normalExport = vec3(-firstDerivative, 1.f, 0.f);

	viewPosition = viewPos;
	fragPos = vec3(model * vec4(waterCoords, 1.f));
}