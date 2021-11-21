#version 420 core

vec4 mixTextures(float lower, float higher, float max, float blendFactor, float blendRange);
vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir);

in EXPORT
{
	vec3 norm;
	vec3 colors;
	vec3 viewPosition;
	vec3 fragPos;
} vs_export;

in 	vec2 uv_frag;
in	float height_frag;

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

vec3 normal, lightDirection, view;

vec4 textureColour;

out vec4 colorsOut;

void main()
{
	if (length(vs_export.colors) > 0.0){
		colorsOut = vec4(vs_export.colors, 1.0);
	}
	else {
		textureColour = mixTextures(lower, higher, maximum, 0.6f, 1.2f);

		normal = normalize(vs_export.norm);
		lightDirection = normalize(position);
		view = normalize(vs_export.viewPosition - vs_export.fragPos);

		vec3 result = calcLight(normal, lightDirection, view);

		colorsOut = vec4(result, 1.0);
	}
}

vec4 mixTextures(float low, float high, float max, float blendFactor, float blendRange){
	vec4 result;

	vec4 sandColour = texture(u_texture1,	uv_frag);
	vec4 grassColour = texture(u_texture2,	uv_frag);
	vec4 rockColour = texture(u_texture3,	uv_frag);
	vec4 snowColour = texture(u_texture4,	uv_frag);

	if(height_frag < low){ result = sandColour;}
	if (height_frag > low - blendRange && height_frag < low + blendRange) {
		result = mix(sandColour, grassColour, blendFactor);
	}

	if(height_frag > low && height_frag < high){ result = grassColour;}
	if (height_frag > high - blendRange && height_frag < high + blendRange) {
		result = mix(grassColour, rockColour, blendFactor);
	}

	if(height_frag > high && height_frag < max){ result = rockColour;}
	if (height_frag > max - blendRange && height_frag < max + blendRange) {
		result = mix(rockColour, snowColour, blendFactor);
	}

	if(height_frag > max){ result = snowColour;}

	return result;
}

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir) {
	float diff = max(dot(n, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.0);

	vec3 ambient = ambient * vec3(textureColour);
	vec3 diffuse = diffuse * diff * vec3(textureColour) * 1.3;
	vec3 specular = specular * spec * vec3(textureColour) * 0.0;

	return (ambient + diffuse + specular);
}