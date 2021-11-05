#version 420 core

flat in vec3 colorsExport;
in vec3 normalExport;
in vec2 uvExport;
in float heightExport;

layout(std140, binding = 1) uniform light 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

uniform sampler2D u_texture;

vec3 normal, lightDirection;
vec3 fAndBDiff;
vec4 textureColour;

out vec4 colorsOut;

void main(void)
{
	colorsOut = vec4(colorsExport, 1.0);
	normal = normalize(normalExport);
	lightDirection = normalize(position);
	fAndBDiff = max(dot(normal, lightDirection), 0.0) * diffuse * specular;
	textureColour = texture(u_texture, uvExport);

	colorsOut = vec4(min(fAndBDiff, vec3(1.0)), 1.0) * textureColour;
}