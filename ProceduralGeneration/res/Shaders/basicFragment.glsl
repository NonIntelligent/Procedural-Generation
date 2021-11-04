#version 420 core

flat in vec3 colorsExport;
in vec3 normalExport;

layout(std140) uniform light 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

vec3 normal, lightDirection;
vec3 fAndBDiff;

out vec4 colorsOut;

void main(void)
{
	colorsOut = vec4(colorsExport, 1.0);
	normal = normalize(normalExport);
	lightDirection = normalize(position);
	fAndBDiff = max(dot(normal, lightDirection), 0.f) * diffuse * specular;
	colorsOut = vec4(colorsExport * min(fAndBDiff, vec3(1.0)), 1.0);
}