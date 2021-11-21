#version 420 core

layout(location=0) in vec3 grassCoord;
layout(location=1) in vec3 grassColor;
layout(location=2) in vec3 grassNormal;
layout(location=3) in vec2 grassUV;

layout(std140, binding = 0) uniform camera
{
	mat4 proj;
	mat4 view;
	vec3 viewPos;
};

uniform mat4 model;
uniform vec4 plane;

out EXPORT
{
	vec3 norm;
	vec3 colors;
	vec3 viewPosition;
	vec3 fragPos;
} export;

out 	vec2 uv;
out		float height;

void main()
{
	vec4 pos = vec4(grassCoord, 1.0);

	gl_ClipDistance[0] = dot(pos, plane);

	export.colors = vec3(0.0, 1.0, 0.0);
	export.norm = normalize(grassNormal);
	uv = grassUV;
	height = grassCoord.y;

	export.viewPosition = viewPos;
	export.fragPos = vec3(model * vec4(grassCoord, 1.f));

	gl_Position = proj * view * model * pos;
}