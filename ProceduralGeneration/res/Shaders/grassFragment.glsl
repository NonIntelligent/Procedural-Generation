#version 420 core

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir);

in EXPORT
{
	vec3 norm;
	vec3 colors;
	vec3 viewPosition;
	vec3 fragPos;
} gs_export;

in 	vec2 uv_frag;
in	float height_frag;

layout(std140, binding = 1) uniform light 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

out vec4 colorsOut;

void main(void)
{
	normal = normalize(gs_export.norm);
	lightDirection = normalize(position);
	view = normalize(gs_export.viewPosition - gs_export.fragPos);

	vec3 result;

	result = calcLight(normal, lightDirection, view);

	colorsOut = vec4(result, 1.0);
}

vec3 calcLight(vec3 n, vec3 lightDir, vec3 viewDir) {
	float diff = max(dot(n, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.0);

	vec3 ambient = ambient * gs_export.colors;
	vec3 diffuse = diffuse * diff * gs_export.colors * 1.0;
	vec3 specular = specular * spec * gs_export.colors * 0.3;

	return ambient + diffuse + specular;
}