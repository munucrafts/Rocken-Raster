#include "Primitives.h"
#include <imgui.h>

struct Camera
{
private:
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstMouse = true;
	float speedMultiplier = 0.1f;
	float mouseSensitivity = 0.1f;
	glm::vec2 lastMousePos = glm::vec2(0.0f);

public:
	float fov = glm::radians(60.0f);
	Transform transform;
	SpeedComponent speedComp;

private:
	void RotateCamera(float deltaTime)
	{
		transform.rotation += speedComp.angularSpeed * deltaTime;
		transform.rotation.x = glm::clamp(transform.rotation.x, -89.0f, 89.0f);
	};
	void MoveCamera(float deltaTime)
	{
		transform.location += speedComp.linearSpeed * deltaTime;
	};
	void UpdateCameraSpeed(float step)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseWheel > 0.0f) speedMultiplier += step;
		else if (io.MouseWheel < 0.0f) speedMultiplier -= step;
		speedMultiplier = glm::clamp(speedMultiplier, 0.01f, 0.6f);
	}
	glm::vec3 GetDirection()
	{
		glm::vec3 dir;
		dir.x = cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
		dir.y = sin(glm::radians(transform.rotation.x));
		dir.z = sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
		return dir;
	};

public:
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(transform.location, transform.location + forward, up);
	};
	void NavigateCamera(float deltaTime)
	{
		if (!Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right))
		{
			firstMouse = true;
			return;
		}
		UpdateCameraSpeed(0.1f);

		{	
			if (Walnut::Input::IsKeyDown(Walnut::Key::A))
			{
				speedComp.linearSpeed = -right * speedMultiplier;
				MoveCamera(deltaTime);
			}
			if (Walnut::Input::IsKeyDown(Walnut::Key::D))
			{
				speedComp.linearSpeed = right * speedMultiplier;
				MoveCamera(deltaTime);
			}
			if (Walnut::Input::IsKeyDown(Walnut::Key::E))
			{
				speedComp.linearSpeed = up * speedMultiplier;
				MoveCamera(deltaTime);
			}
			if (Walnut::Input::IsKeyDown(Walnut::Key::Q))
			{
				speedComp.linearSpeed = -up * speedMultiplier;
				MoveCamera(deltaTime);
			}
			if (Walnut::Input::IsKeyDown(Walnut::Key::W))
			{
				speedComp.linearSpeed = forward * speedMultiplier;
				MoveCamera(deltaTime);
			}
			if (Walnut::Input::IsKeyDown(Walnut::Key::S))
			{
				speedComp.linearSpeed = -forward * speedMultiplier;
				MoveCamera(deltaTime);
			}
		}

		{	
			glm::vec2 mousePos = Walnut::Input::GetMousePosition();

			if (firstMouse)
			{
				lastMousePos = mousePos;
				firstMouse = false;
				return;
			}

			glm::vec2 delta = mousePos - lastMousePos;
			lastMousePos = mousePos;

			speedComp.angularSpeed = glm::vec3(-delta.y * mouseSensitivity, delta.x * mouseSensitivity, 0.0f);
			RotateCamera(deltaTime);

			glm::vec3 direction = GetDirection();
			forward = glm::normalize(direction);
			right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
			up = glm::normalize(glm::cross(right, forward));
		}
	}
};