#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <Walnut/Input/Input.h>
#include "../../Walnut/vendor/stb_image/stb_image.h"

enum Projection
{
	ORTHOGRAPHIC, PERSPECTIVE
};

enum ViewMode
{
	LIT, UNLIT, TRIAGULATE, DEPTH, NORMAL
};

enum Mobility
{
	Static, Movable
};

struct Vertex
{
	glm::vec3 vert;
	glm::vec2 uv;
	glm::vec3 normal;
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

struct Entity
{
	Entity() = default;
	virtual ~Entity() = default;

	virtual void RotateEntity(float deltaTime) {};
	virtual void MoveEntity(float deltaTime) {};
	virtual void ScaleEntity(float deltaTime) {};

	Mobility mobility = Static;
};

struct Texture
{
private:
	int texWidth = 0, texHeight = 0, channels = 0;

public:
	uint8_t* texData = nullptr;
	void LoadTextureFile(std::string& texPath)
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
	glm::vec4 RandomColor()
	{
		static std::mt19937 rng(std::random_device{}());
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return glm::vec4(dist(rng), dist(rng), dist(rng), 1.0f);
	}
	bool hasTex()
	{
		return texture.texData;
	};

	Texture texture;
};

struct Mesh : public Entity
{
	Mesh() = default;
	Mesh(Mobility mob)
	{
		mobility = mob;
		isMoving = mobility == Movable;
	};
	virtual ~Mesh() = default;

	std::vector<Triangle> triangles;
	std::vector<Triangle> newTriangles;
	Transform transform;
	SpeedComponent speedComp;
	Material mat;

	glm::mat4 bakedTransform = glm::mat4(0.0f);
	bool isMoving = false;

	void RotateEntity(float deltaTime) override
	{
		transform.rotation += speedComp.angularSpeed * deltaTime;
		isMoving = true;
	};
	void MoveEntity(float deltaTime) override
	{
		transform.location += speedComp.linearSpeed * deltaTime;
		isMoving = true;
	};
	void ScaleEntity(float deltaTime) override
	{
		transform.scale += speedComp.scalingSpeed * deltaTime;
		isMoving = true;
	};
	void LoadObjectFile(std::string objPath, std::string texPath)
	{
		std::ifstream input(objPath);
		if (!input.is_open())
			return;

		std::string line;
		std::vector<glm::vec3> vertexPoints;
		std::vector<glm::vec3> normals;
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
			else if (line.rfind("vn", 0) == 0)
			{
				std::string vn;
				float i, j, k;
				iss >> vn >> i >> j >> k;
				normals.push_back({ i, j, k });
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

				size_t aSlash1 = a.find('/');
				size_t aSlash2 = a.find('/', aSlash1 + 1);
				int aInd = std::stoi(a.substr(0, aSlash1)) - 1;
				int aTexInd = std::stoi(a.substr(aSlash1 + 1, aSlash2 - aSlash1 - 1)) - 1;
				int aNorInd = std::stoi(a.substr(aSlash2 + 1)) - 1;

				size_t bSlash1 = b.find('/');
				size_t bSlash2 = b.find('/', bSlash1 + 1);
				int bInd = std::stoi(b.substr(0, bSlash1)) - 1;
				int bTexInd = std::stoi(b.substr(bSlash1 + 1, bSlash2 - bSlash1 - 1)) - 1;
				int bNorInd = std::stoi(b.substr(bSlash2 + 1)) - 1;

				size_t cSlash1 = c.find('/');
				size_t cSlash2 = c.find('/', cSlash1 + 1);
				int cInd = std::stoi(c.substr(0, cSlash1)) - 1;
				int cTexInd = std::stoi(c.substr(cSlash1 + 1, cSlash2 - cSlash1 - 1)) - 1;
				int cNorInd = std::stoi(c.substr(cSlash2 + 1)) - 1;

				std::vector<Vertex> verts =
				{
					{ vertexPoints[aInd], uvs[aTexInd], normals[aNorInd] },
					{ vertexPoints[bInd], uvs[bTexInd], normals[bNorInd] },
					{ vertexPoints[cInd], uvs[cTexInd], normals[cNorInd] }
				};

				triangles.push_back({ verts });
			}
		}

		mat.texture.LoadTextureFile(texPath);
	};
	bool Retriangulate(int clipCount)
	{
		if (clipCount == 0) return false;
		else if (clipCount == 3) return true;
	}
};

struct Scene
{
	std::vector<Entity*> entities;
};