#version 420 core

layout(location=0) in vec3 grassCoord;
layout(location=1) in vec2 grassUV;
layout(location=2) in mat4 matrix;
layout(location=6) in float random;

layout(std140, binding = 0) uniform camera
{
	mat4 proj;
	mat4 view;
	vec3 viewPos;
};

uniform mat4 model;
uniform vec4 plane;
uniform float windTime;

out EXPORT
{
	vec3 norm;
	vec3 viewPosition;
	vec3 fragPos;
} export;


out 	vec2 uv;


const float windStrength = 2.f;

void main()
{
	vec4 pos = vec4(grassCoord, 1.0);

	gl_ClipDistance[0] = dot(pos, plane);

	vec4 wind = vec4(0.0);
	wind.x += sin(1.5 * random * windTime);

	float grassHeightScalar = grassCoord.y * 0.2;

	if (grassHeightScalar < 0.0) grassHeightScalar = 0.0;

	wind.x *= windStrength * grassHeightScalar * grassHeightScalar;

	wind.x *= random;

	uv = grassUV;

	export.viewPosition = viewPos;
	export.fragPos = vec3(model * vec4(grassCoord, 1.f));

	gl_Position = proj * view * model * matrix * (pos + wind);
}