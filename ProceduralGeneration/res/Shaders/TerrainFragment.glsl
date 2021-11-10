#version 420 core

vec4 mixTextures(float lower, float higher, float max, float blendFactor, float blendRange);

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

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform sampler2D u_texture3;
uniform sampler2D u_texture4;

uniform float maximum, higher, lower;

vec3 normal, lightDirection;
vec3 fAndBDiff;
vec4 textureColour;

out vec4 colorsOut;

void main(void)
{
	textureColour = mixTextures(lower, higher, maximum, 0.6f, 1.2f);

	colorsOut = vec4(colorsExport, 1.0);
	normal = normalize(normalExport);
	lightDirection = normalize(position);
	fAndBDiff = max(dot(normal, lightDirection), 0.0) * diffuse * specular;

	colorsOut = vec4(min(fAndBDiff, vec3(1.0)), 1.0) * textureColour;
}

vec4 mixTextures(float low, float high, float max, float blendFactor, float blendRange){
	vec4 result;

	vec4 sandColour = texture(u_texture1, uvExport);
	vec4 grassColour = texture(u_texture2, uvExport);
	vec4 rockColour = texture(u_texture3, uvExport);
	vec4 snowColour = texture(u_texture4, uvExport);

	if(heightExport < low){ result = sandColour;}
	if (heightExport > low - blendRange && heightExport < low + blendRange) {
		result = mix(sandColour, grassColour, blendFactor);
	}

	if(heightExport > low && heightExport < high){ result = grassColour;}
	if (heightExport > high - blendRange && heightExport < high + blendRange) {
		result = mix(grassColour, rockColour, blendFactor);
	}

	if(heightExport > high && heightExport < max){ result = rockColour;}
	if (heightExport > max - blendRange && heightExport < max + blendRange) {
		result = mix(rockColour, snowColour, blendFactor);
	}

	if(heightExport > max){ result = snowColour;}

	return result;
}