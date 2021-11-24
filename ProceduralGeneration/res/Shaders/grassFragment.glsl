#version 420 core

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir);

in EXPORT
{
	vec3 norm;
	vec3 viewPosition;
	vec3 fragPos;
} vs_export;

in	vec3 colors;
in 	vec2 uv;

layout(std140, binding = 1) uniform light 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

vec3 normal, lightDirection, view;

uniform sampler2D u_texture;

out vec4 colorsOut;

void main()
{
	normal = vec3(0.0, 1.0, 0.0);
	lightDirection = normalize(position);
	view = normalize(vs_export.viewPosition - vs_export.fragPos);

	vec3 result;

	result = calcLight(normal, lightDirection, view);

	colorsOut = vec4(result, 1.0);
}

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir) {
	vec4 textureColour = texture(u_texture, uv);
	float diff = max(dot(n, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.0);

	vec3 ambient = ambient * vec3(textureColour);
	vec3 diffuse = diffuse * diff * vec3(textureColour) * 1.0;
	vec3 specular = specular * spec * vec3(textureColour) * 0.3;

	return ambient + diffuse + specular;
}