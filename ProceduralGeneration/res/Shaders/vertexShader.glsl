#version 420 core

layout(location=0) in vec3 terrainCoords;
layout(location=1) in vec3 terrainNormal;
layout(location=2) in vec2 terrainUV;

layout(std140, binding = 0) uniform camera
{
	mat4 proj;
	mat4 view;
	vec3 viewPos;
};

uniform mat4 model;
uniform mat3 normalMat;
uniform vec4 plane;

out EXPORT
{
	vec3 norm;
	vec3 viewPosition;
	vec3 fragPos;
} export;

out 	vec2 uv;
out		float height;

void main()
{
	vec4 pos = vec4(terrainCoords, 1.0);

	gl_ClipDistance[0] = dot(pos, plane);

	export.norm = normalize(normalMat * terrainNormal);
	uv = terrainUV;
	height = terrainCoords.y;

	export.viewPosition = viewPos;
	export.fragPos = vec3(model * vec4(terrainCoords, 1.f));

	gl_Position = proj * view * model * pos;
}