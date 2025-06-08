#include "Renderer.h"

void Renderer::Render(float width, float height, float deltaSecs)
{
	if (image)
	{
		if (width != image->GetWidth() || height != image->GetHeight())
		{
			image->Resize(width, height);
			imageData.resize(width * height);
			screenResolution = glm::vec2(width, height);
		}
	}
	else
	{
		image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		imageData.resize(width * height);
		screenResolution = glm::vec2(width, height);
	}

	ClearBackground(glm::vec4(0.25f));

	Mesh testMesh;
	testMesh.triangles.push_back({ glm::vec2(-0.25f, -0.25f), glm::vec2(0.25f, -0.25f), glm::vec2(0.0f, 0.25f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) });
	scene.meshes.push_back(testMesh);

	if (scene.meshes.empty())
		return;

	for (int y = 0; y < screenResolution.y; y++)
	{
		for (int x = 0; x < screenResolution.x; x++)
		{
			for (Mesh& mesh : scene.meshes)
			{
				for (Triangle& tri : mesh.triangles)
				{
					DrawTriangle(tri, glm::vec2(x, y));
				}
			}
		}
	}

	image->SetData(imageData.data());
}

uint32_t Renderer::ColorToRGBA(glm::vec4 color)
{
	glm::vec4 clampedColor = 255.0f * glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
	uint8_t r = (uint8_t)clampedColor.x;
	uint8_t g = (uint8_t)clampedColor.y;
	uint8_t b = (uint8_t)clampedColor.z;
	uint8_t a = (uint8_t)clampedColor.a;
	uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
	return result;
}

bool Renderer::OnRightSideOfLine(glm::vec2 a, glm::vec2 b, glm::vec2 p)
{
	glm::vec2 ap = p - a;
	glm::vec2 ab = b - a;
	glm::vec2 abPerp = glm::vec2(ab.y, -ab.x);
	return glm::dot(abPerp, ap) >= 0;
}

bool Renderer::InsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p)
{
	bool sideAB = OnRightSideOfLine(a, b, p);
	bool sideBC = OnRightSideOfLine(b, c, p);
	bool sideCA = OnRightSideOfLine(c, a, p);
	return sideAB == sideBC && sideBC == sideCA;
}

glm::vec2 Renderer::UVToPixel(glm::vec2 q)
{
	q = q * 0.5f + 0.5f;
	q = q * screenResolution;

	return q;
}

void Renderer::ClearBackground(glm::vec4 bgColor)
{
	std::fill(imageData.begin(), imageData.end(), ColorToRGBA(bgColor));
}

void Renderer::DrawPixel(glm::vec2 pixelLoc, glm::vec4 pixelColor)
{
	if (pixelLoc.x >= 0 && pixelLoc.x < screenResolution.x && pixelLoc.y >= 0 && pixelLoc.y < screenResolution.y)
		imageData[pixelLoc.x + pixelLoc.y * image->GetWidth()] = ColorToRGBA(pixelColor);
}

void Renderer::DrawTriangle(Triangle& tri, glm::vec2 currentPixel)
{
	glm::vec2 a = UVToPixel(tri.a);;
	glm::vec2 b = UVToPixel(tri.b);;
	glm::vec2 c = UVToPixel(tri.c);;

	if (InsideTriangle(a, b, c, currentPixel))
		DrawPixel(currentPixel, tri.color);
}

std::shared_ptr<Walnut::Image>& Renderer::GetImage()
{
	return image;
}
