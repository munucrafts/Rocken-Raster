#include "Renderer.h"

Renderer::Renderer()
{
	deltaTime = 0.0f;
	screenResolution = glm::vec2(0.0f);
	projection = ORTHOGRAPHIC;

	Mesh testMesh;
	testMesh.LoadObjectFile("../Assets/Cube.obj");
	scene.meshes.push_back(testMesh);
}

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

	deltaTime = deltaSecs / 10000.0f;
	ClearBackground(glm::vec4(0.25f));

	if (scene.meshes.empty())
		return;

	for (Mesh& mesh : scene.meshes)
	{
		mesh.Rotate(glm::vec3(0.5f, 0.5f, 0.0f), deltaTime);

		for (Triangle& tri : mesh.triangles)
		{
			glm::vec3 ab = tri.b - tri.a;
			glm::vec3 ac = tri.c - tri.a;
			glm::vec3 normal = glm::cross(ab, ac);

			if (glm::dot(normal, glm::vec3(0.0f, 0.0f, 1.0f)) > 0.0f)
				continue;

			glm::vec2 a = WorldToUV(tri.a, mesh.transform);
			glm::vec2 b = WorldToUV(tri.b, mesh.transform);
			glm::vec2 c = WorldToUV(tri.c, mesh.transform);

			UVToPixel(a); 
			UVToPixel(b); 
			UVToPixel(c); 

			BoundingBox box = GetTriangleBoundingBox(a, b, c);

			int minPixelX = (int)box.minMaxX.x;
			int maxPixelX = (int)box.minMaxX.y;
			int minPixelY = (int)box.minMaxY.x;
			int maxPixelY = (int)box.minMaxY.y;

			for (int y = minPixelY; y < maxPixelY; y++)
			{
				for (int x = minPixelX; x < maxPixelX; x++)
				{
					if (InsideTriangle(a, b, c, glm::vec2(x, y)))
						DrawPixel(glm::vec2(x, y), tri.color);
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

void Renderer::UVToPixel(glm::vec2& q)
{
	q = q * 0.5f + 0.5f;
	q = q * screenResolution;
	q = glm::round(q);
}

void Renderer::PixelToUV(glm::vec2& q)
{
	q = q / screenResolution;
	q = q * 2.0f - 1.0f;
}

glm::vec2 Renderer::WorldToUV(glm::vec3& point, Transform& objectTransform)
{
	float aspectRatio = screenResolution.x / screenResolution.y;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, objectTransform.location);
	model = glm::rotate(model, glm::radians(objectTransform.rotation.y), glm::vec3(1, 1, 1));
	model = glm::scale(model, objectTransform.scale);

	switch (projection)
	{
	case ORTHOGRAPHIC:
	{
		glm::mat4 orthoMat = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f);
		glm::mat4 transform = orthoMat * model;
		glm::vec4 transformed = transform * glm::vec4(point, 1.0f);
		return glm::vec2(transformed.x, transformed.y);
	}
	case PERSPECTIVE:
		return glm::vec2(0.0f);
	}
}



void Renderer::ClearBackground(glm::vec4 bgColor)
{
	std::fill(imageData.begin(), imageData.end(), ColorToRGBA(bgColor));
}

void Renderer::DrawPixel(glm::vec2 pixelLoc, glm::vec4 pixelColor)
{
	if (pixelLoc.x >= 0 && pixelLoc.x <= screenResolution.x && pixelLoc.y >= 0 && pixelLoc.y <= screenResolution.y)
		imageData[pixelLoc.x + pixelLoc.y * image->GetWidth()] = ColorToRGBA(pixelColor);
}

BoundingBox Renderer::GetTriangleBoundingBox(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
	int minX = std::max(0, (int)std::floor(std::min({ a.x, b.x, c.x })));
	int maxX = std::min((int)screenResolution.x, (int)std::ceil(std::max({ a.x, b.x, c.x })));

	int minY = std::max(0, (int)std::floor(std::min({ a.y, b.y, c.y })));
	int maxY = std::min((int)screenResolution.y, (int)std::ceil(std::max({ a.y, b.y, c.y })));

	return { glm::vec2(minX, maxX), glm::vec2(minY, maxY) };
}

std::shared_ptr<Walnut::Image>& Renderer::GetImage()
{
	return image;
}