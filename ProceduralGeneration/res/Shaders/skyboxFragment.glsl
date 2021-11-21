#version 420 core

in vec3 texCoords;

uniform samplerCube u_texture;

out vec4 colour;

void main() {
	colour = texture(u_texture, texCoords);
};