#version 420 core

flat in vec3 colorsExport;
in vec3 normalExport;
in vec2 uvExport;
in float heightExport;

in vec3 viewPosition;
in vec3 fragPos;

layout(std140, binding = 1) uniform light 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

vec3 normal, lightDirection, view;

out vec4 colorsOut;

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir);

void main(void)
{
	colorsOut = vec4(colorsExport, 1.0);
	normal = normalize(normalExport);
	lightDirection = normalize(position);
	view = normalize(viewPosition - fragPos);

	vec3 result = calcLight(normal, lightDirection, view);

	colorsOut = vec4(result, 0.8);
}

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir) {
	float diff = max(dot(n, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);

	vec3 ambient = ambient * colorsExport;
	vec3 diffuse = diffuse * diff * colorsExport;
	vec3 specular = specular * spec * colorsExport;

	return (ambient + diffuse + specular);
}