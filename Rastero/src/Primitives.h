#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <Walnut/Input/Input.h>
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

struct Camera
{
private:
	Transform transform;
	SpeedComponent speedComp;
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstMouse = true;
	float speedMultiplier = 0.1f;
	float mouseSensitivity = 0.1f;
	glm::vec2 lastMousePos = glm::vec2(0.0f);

public:
	float fov = glm::radians(60.0f);

private:
	void AddRotation(float deltaTime)
	{
		transform.rotation += speedComp.angularSpeed * deltaTime;
	};
	void AddLocation(float deltaTime)
	{
		transform.location += speedComp.linearSpeed * deltaTime;
	};

public:
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(transform.location, transform.location + forward, up);
	};
	void Navigate(float deltaTime)
	{
		if (!Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right))
		{
			firstMouse = true;
			return;
		}
		if (Walnut::Input::IsKeyDown(Walnut::Key::A))
		{
			speedComp.linearSpeed = -right * speedMultiplier;
			AddLocation(deltaTime);
		}
		if (Walnut::Input::IsKeyDown(Walnut::Key::D))
		{
			speedComp.linearSpeed = right * speedMultiplier;
			AddLocation(deltaTime);
		}
		if (Walnut::Input::IsKeyDown(Walnut::Key::E))
		{
			speedComp.linearSpeed = up * speedMultiplier;
			AddLocation(deltaTime);
		}
		if (Walnut::Input::IsKeyDown(Walnut::Key::Q))
		{
			speedComp.linearSpeed = -up * speedMultiplier;
			AddLocation(deltaTime);
		}
		if (Walnut::Input::IsKeyDown(Walnut::Key::W))
		{
			speedComp.linearSpeed = forward * speedMultiplier;
			AddLocation(deltaTime);
		}
		if (Walnut::Input::IsKeyDown(Walnut::Key::S))
		{
			speedComp.linearSpeed = -forward * speedMultiplier;
			AddLocation(deltaTime);
		}

		{
			glm::vec2 mousePos = Walnut::Input::GetMousePosition();

			if (firstMouse)
			{
				lastMousePos = mousePos;
				firstMouse = false;
			}

			glm::vec2 delta = mousePos - lastMousePos;
			lastMousePos = mousePos;

			

			float& yaw = transform.rotation.y;
			float& pitch = transform.rotation.x;

			yaw += delta.x * mouseSensitivity;
			pitch -= delta.y * mouseSensitivity;
			pitch = glm::clamp(pitch, -89.0f, 89.0f);

			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

			forward = glm::normalize(direction);
			right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
			up = glm::normalize(glm::cross(right, forward));
		}
	}
};

struct Mesh
{
	std::vector<Triangle> triangles;
	Transform transform;
	SpeedComponent speedComp;

	void AddRotation(float deltaTime)
	{
		transform.rotation += speedComp.angularSpeed * deltaTime;
	};

	void AddLocation(float deltaTime)
	{
		transform.location += speedComp.linearSpeed * deltaTime;
	};

	void AddScale(float deltaTime)
	{
		transform.scale += speedComp.scalingSpeed * deltaTime;
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