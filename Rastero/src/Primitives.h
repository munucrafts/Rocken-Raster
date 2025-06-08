#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Triangle
{
	glm::vec2 a;
	glm::vec2 b;
	glm::vec2 c;
	glm::vec4 color;
};

struct Mesh
{
	std::vector<Triangle> triangles;
};

struct Scene
{
	std::vector<Mesh> meshes;
};