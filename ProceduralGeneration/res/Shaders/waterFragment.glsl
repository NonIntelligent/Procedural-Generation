#version 420 core

flat in vec3 colorsExport;
in vec3 normalExport;
in vec2 uvExport;

in vec3 viewPosition;
in vec3 fragPos;
in vec4 clipSpace;
in float time;

layout(std140, binding = 1) uniform light 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

vec3 normal, lightDirection, view;

const float distortStrength = 0.025f;

uniform sampler2D u_reflection, u_refraction, u_depth;

out vec4 colorsOut;

vec3 calcSpec(vec3 n, vec3 lightDir, vec3 viewDir);

void main(void)
{
	normal = normalize(normalExport);

	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.f + 0.5f;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = ndc;

	float near = 1.0;
	float far = 1000.0;
	float depth = texture(u_depth, refractTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	float waterDepth = floorDistance - waterDistance;

	vec2 distortion = vec2(normal.x, normal.y) * distortStrength * clamp(waterDepth / 10.0, 0.0, 1.0);

	reflectTexCoords += distortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	refractTexCoords += distortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 reflectColour = texture(u_reflection, reflectTexCoords);
	vec4 refractColour = texture(u_refraction, refractTexCoords);
	refractColour = mix(refractColour, vec4(0.0, 0.05, 0.25, 1.0), clamp(waterDepth / 120.0, 0.0, 1.0));

	lightDirection = normalize(position);
	view = normalize(viewPosition - fragPos);

	vec3 result = calcSpec(normal, lightDirection, view);

	colorsOut = mix(reflectColour, refractColour, 0.5);
	colorsOut = mix(colorsOut, vec4(0.0, 0.3, 0.75, 1.0), 0.25);
	colorsOut.a = clamp(waterDepth / 5.0, 0.0, 1.0);
}

vec3 calcSpec(vec3 n, vec3 lightDir, vec3 viewDir) {

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);

	vec3 specular = specular * spec * colorsExport;

	return (specular);
}