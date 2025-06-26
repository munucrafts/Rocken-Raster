#include <imgui.h>
#include "Primitives.h"

struct Camera
{
private:
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstMouse = true;
	float speedMultiplier = 0.1f;
	float mouseSensitivity = 0.075f;
	glm::vec2 lastMousePos = glm::vec2(0.0f);

public:
	float fov = glm::radians(60.0f);
	Transform transform;
	SpeedComponent speedComp;
	float orthoValue = 10.0f;
	bool isDirty = false;

private:
	void RotateCamera(float deltaTime)
	{
		transform.rotation += speedComp.angularSpeed * deltaTime;
		transform.rotation.x = glm::clamp(transform.rotation.x, -89.0f, 89.0f);
		UpdateDirectionVectors();
		isDirty = true;
	};
	void MoveCamera(float deltaTime)
	{
		transform.location += speedComp.linearSpeed * deltaTime;
		isDirty = true;
	};
	void UpdateCameraSpeed(float step)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseWheel == 0.0f) return;

		if (io.MouseWheel > 0.0f)
		{
			speedMultiplier += step;
			speedMultiplier = glm::clamp(speedMultiplier, 0.01f, 0.6f);
		}
		else if (io.MouseWheel < 0.0f)
		{
			speedMultiplier -= step;
			speedMultiplier = glm::clamp(speedMultiplier, 0.01f, 0.6f);
		}
	}
	void UpdateDirectionVectors()
	{
		glm::vec3 direction;

		float yaw = glm::radians(transform.rotation.y - 90.0f);   
		float pitch = glm::radians(transform.rotation.x);         

		direction.x = cos(yaw) * cos(pitch);
		direction.y = sin(pitch);
		direction.z = sin(yaw) * cos(pitch);

		forward = glm::normalize(direction);
		right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		up = glm::normalize(glm::cross(right, forward));
	};
	void PanCamera(float deltaTime)
	{
		transform.location.x += speedComp.linearSpeed.x * deltaTime;
		transform.location.y += speedComp.linearSpeed.y * deltaTime;
		isDirty = true;
	};
	void OrthographicZoom(float step) 
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseWheel == 0.0f) return;

		if (io.MouseWheel > 0.0f)
		{
			orthoValue -= step;
			orthoValue = glm::clamp(orthoValue, 5.0f, 100.0f);
		}
		else if (io.MouseWheel < 0.0f)
		{
			orthoValue += step;
			orthoValue = glm::clamp(orthoValue, 5.0f, 100.0f);
		}
	};

public:
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(transform.location, transform.location + forward, up);
	};
	void NavigateCamera(float deltaTime, Projection& projType)
	{
		isDirty = false;

		if (projType == PERSPECTIVE)
		{
			if (!Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right))
			{
				firstMouse = true;
				return;
			}

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

			UpdateCameraSpeed(0.1f);

			glm::vec2 mousePos = Walnut::Input::GetMousePosition();

			if (firstMouse)
			{
				lastMousePos = mousePos;
				firstMouse = false;
				return;
			}

			glm::vec2 delta = mousePos - lastMousePos;

			if (glm::length(delta) < 0.1f) return;

			lastMousePos = mousePos;

			speedComp.angularSpeed = glm::vec3(-delta.y * mouseSensitivity, delta.x * mouseSensitivity, 0.0f);
			RotateCamera(deltaTime);
		}
		else if (projType == ORTHOGRAPHIC);
	}
};