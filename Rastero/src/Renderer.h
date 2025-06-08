#pragma once
#include <Walnut/Image.h>
#include <vector>
#include <memory>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include "Primitives.h"

class Renderer
{
public:
	Renderer() = default;
	void Render(float width, float height, float deltaSecs);
	std::shared_ptr<Walnut::Image>& GetImage();

private:
	uint32_t ColorToRGBA(glm::vec4 color);
	bool OnRightSideOfLine(glm::vec2 a, glm::vec2 b, glm::vec2 p);
	bool InsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p);
	glm::vec2 UVToPixel(glm::vec2 q);
	void ClearBackground(glm::vec4 bgColor);

	void DrawPixel(glm::vec2 pixelLoc, glm::vec4 pixelColor);
	void DrawTriangle(Triangle& tri, glm::vec2 currentPixel);

private:
	std::vector<uint32_t> imageData;
	std::shared_ptr<Walnut::Image> image;
	glm::vec2 screenResolution;
	Scene scene;
};

