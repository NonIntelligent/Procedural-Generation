#ifndef VERTEX_H
#define VERTEX_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Vertex
{
	glm::vec4 coords;
	glm::vec2 texCoords;
};

struct VertexWtihNormal
{
	glm::vec4 coords;
	glm::vec3 normals;
};

struct  VertexWithAll
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 textcoord;
};

#endif