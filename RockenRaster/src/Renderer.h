#pragma once
#include <Walnut/Image.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Primitives.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer();
	void Render(float width, float height, float delta);
	std::shared_ptr<Walnut::Image>& GetImage();

private:
	uint32_t ColorToRGBA(glm::vec4& color);
	float GetSignedTriangleArea(glm::vec2& a, glm::vec2& b, glm::vec2& p);
	bool InsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p, glm::vec3& weights);
	void NDCToPixel(glm::vec3& q);
	void PixelToNDC(glm::vec2& q);
	glm::vec4 WorldToNDC(glm::vec3& point, glm::mat4& model);
	glm::mat4 ModelToWorld(Transform& objectTransform);
	void ClearBackground(glm::vec4& bgColor);
	BoundingBox GetTriangleBoundingBox(glm::vec3& a, glm::vec3& b, glm::vec3& c);
	void DrawPixel(glm::vec2 pixelLoc, glm::vec4 pixelColor);
	void ResetDepthBuffer();

private:
	std::vector<uint32_t> imageData;
	std::vector<float> depthBuffer;
	std::shared_ptr<Walnut::Image> image;
	glm::vec2 screenResolution;
	Scene scene;
	Camera camera;
	Projection projection;
	float deltaTime;
};

