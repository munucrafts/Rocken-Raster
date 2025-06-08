#pragma once
#include <Walnut/Image.h>
#include <vector>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Primitives.h"

class Renderer
{
public:
	Renderer();
	void Render(float width, float height, float deltaSecs);
	std::shared_ptr<Walnut::Image>& GetImage();

private:
	uint32_t ColorToRGBA(glm::vec4 color);
	bool OnRightSideOfLine(glm::vec2 a, glm::vec2 b, glm::vec2 p);
	bool InsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p);
	void UVToPixel(glm::vec2& q);
	void PixelToUV(glm::vec2& q);
	glm::vec2 WorldToUV(glm::vec3& point, Transform& transform);
	void ClearBackground(glm::vec4 bgColor);
	BoundingBox GetTriangleBoundingBox(glm::vec2 a, glm::vec2 b, glm::vec2 c);
	void DrawPixel(glm::vec2 pixelLoc, glm::vec4 pixelColor);

private:
	std::vector<uint32_t> imageData;
	std::shared_ptr<Walnut::Image> image;
	glm::vec2 screenResolution;
	Scene scene;
	Projection projection;
	float deltaTime;
};

