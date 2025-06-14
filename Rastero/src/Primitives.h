#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include "Renderer.h"

static glm::vec4 RandomColor()
{
	static std::mt19937 rng(std::random_device{}());
	static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	return glm::vec4(dist(rng), dist(rng), dist(rng), 1.0f);
}

enum Projection
{
	ORTHOGRAPHIC, PERSPECTIVE
};

struct Triangle
{
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec4 color;
};

struct BoundingBox
{
	glm::vec2 minMaxX;
	glm::vec2 minMaxY;
};

struct Transform
{
	glm::vec3 location = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

struct Camera
{
	Transform transform;

	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float fov = glm::radians(45.0f);
};

struct Mesh
{
	std::vector<Triangle> triangles;
	Transform transform;

	void AddRotation(glm::vec3 speed, float deltaTime)
	{
		transform.rotation += speed * deltaTime;
	};

	void AddLocation(glm::vec3 speed, float deltaTime)
	{
		transform.location += speed * deltaTime;
	};

	void AddScale(glm::vec3 speed, float deltaTime)
	{
		transform.scale += speed * deltaTime;
	};

	void LoadObjectFile(std::string filePath)
	{
		std::ifstream input(filePath);
		if (!input.is_open())
			return;

		std::string line;
		std::vector<glm::vec3> vertices;

		while (std::getline(input, line))
		{
			std::istringstream iss(line);
			if (line[0] == 'v')
			{
				char type;
				float x, y, z;
				iss >> type >> x >> y >> z;
				vertices.push_back({ x, y, z });
			}
			else if (line[0] == 'f')
			{
				char type;
				int a, b, c;
				iss >> type >> a >> b >> c;
				glm::vec4 randCol = RandomColor();
				triangles.push_back({ vertices[a - 1], vertices[b - 1], vertices[c - 1], randCol });
			}
		}
	}

};

struct Scene
{
	std::vector<Mesh> meshes;
};