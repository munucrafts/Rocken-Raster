#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <Walnut/Input/Input.h>
#include "Renderer.h"
#include "../../Walnut/vendor/stb_image/stb_image.h"

enum Projection
{
	ORTHOGRAPHIC, PERSPECTIVE
};

struct Texture
{
	int texWidth = 0, texHeight = 0, channels = 0;
	uint8_t* texData = nullptr;

	void LoadTexture(std::string& texPath)
	{
		texData = stbi_load(texPath.c_str(), &texWidth, &texHeight, &channels, 4);
	}
	glm::vec4 LoadColorAtTexureCoordinates(glm::vec2& uv)
	{
		if (!texData) return glm::vec4(1.0f);

		glm::vec2 clampedUV = glm::clamp(uv, glm::vec2(0.0f), glm::vec2(1.0f));
		int x = (int)(clampedUV.x * (texWidth - 1));
		int y = (int)(clampedUV.y * (texHeight - 1));

		int pixelIndex = (x + y * texWidth) * 4;

		uint8_t r = texData[pixelIndex + 0];
		uint8_t g = texData[pixelIndex + 1];
		uint8_t b = texData[pixelIndex + 2];
		uint8_t a = texData[pixelIndex + 3];

		return glm::vec4(r, g, b, a) / 255.0f;
	}
};

struct Material
{
	Texture texture;
	glm::vec4 RandomColor()
	{
		static std::mt19937 rng(std::random_device{}());
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return glm::vec4(dist(rng), dist(rng), dist(rng), 1.0f);
	}
};

struct Vertex
{
	glm::vec3 vert;
	glm::vec2 uv;
};

struct Triangle
{
	std::vector<Vertex> vertices;
};

struct BoundingBox
{
	glm::vec2 minMaxX;
	glm::vec2 minMaxY;
	glm::vec2 minMaxZ;
};

struct Transform
{
	glm::vec3 location = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

struct SpeedComponent
{
	glm::vec3 linearSpeed = glm::vec3(0.0f);
	glm::vec3 angularSpeed = glm::vec3(0.0f);
	glm::vec3 scalingSpeed = glm::vec3(0.0f);
};

struct Mesh
{
	std::vector<Triangle> triangles;
	Transform transform;
	SpeedComponent speedComp;
	Material mat;

	void RotateMesh(float deltaTime)
	{
		transform.rotation += speedComp.angularSpeed * deltaTime;
	};
	void TranslateMesh (float deltaTime)
	{
		transform.location += speedComp.linearSpeed * deltaTime;
	};
	void ScaleMesh(float deltaTime)
	{
		transform.scale += speedComp.scalingSpeed * deltaTime;
	};
	void LoadObjectFile(std::string objPath, std::string texPath)
	{
		std::ifstream input(objPath);
		if (!input.is_open())
			return;

		std::string line;
		std::vector<glm::vec3> vertexPoints;
		std::vector<glm::vec2> uvs;

		while (std::getline(input, line))
		{
			std::istringstream iss(line);

			if (line.rfind("vt", 0) == 0)
			{
				std::string vt;
				float u, v;
				iss >> vt >> u >> v;
				uvs.push_back({ u, 1.0f - v });
			}
			else if (line.rfind("v ", 0) == 0)
			{
				std::string v;
				float x, y, z;
				iss >> v >> x >> y >> z;
				vertexPoints.push_back({ x, y, z });
			}
			else if (line.rfind("f ", 0) == 0)
			{
				std::string f, a, b, c;
				iss >> f >> a >> b >> c;

				int slashA = a.find('/');
				int aInd = std::stoi(a.substr(0, slashA)) - 1;
				int aTexInd = std::stoi(a.substr(slashA + 1)) - 1;

				int slashB = b.find('/');
				int bInd = std::stoi(b.substr(0, slashB)) - 1;
				int bTexInd  = std::stoi(b.substr(slashB + 1)) - 1;

				int slashC = c.find('/');
				int cInd = std::stoi(c.substr(0, slashC)) - 1;
				int cTexInd = std::stoi(c.substr(slashC + 1)) - 1;

				std::vector<Vertex> verts = 
				{
					{ vertexPoints[aInd], uvs[aTexInd] },
					{ vertexPoints[bInd], uvs[bTexInd] },
					{ vertexPoints[cInd], uvs[cTexInd] }
				};

				triangles.push_back({verts});
			}
		}

		mat.texture.LoadTexture(texPath);
	}
};

struct Scene
{
	std::vector<Mesh> meshes;
};