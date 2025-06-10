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
	ClearBackground(glm::vec4(0.2f));

	if (scene.meshes.empty())
		return;

	// Note - Model = Object's Local Transform Transfomred to World Space Using Orthographic or Perspective Projection
	// NDC = Normalized Device Coordinates = Model's World Transform Transformed to UV Space { (0, 0) to (1, 1) } and then to NDC Space { (-1, -1) to (1, 1) }
	// Pixel = Basically Screen Space which is { (0, 0) to (screenResolution.x, screenResolution.y) }

	glm::vec3 camDir = glm::vec3(0.0f, 0.0f, 1.0f);

	for (Mesh& mesh : scene.meshes)
	{
		mesh.Rotate(glm::vec3(0.3f, 0.3f, 0.0f), deltaTime);

		for (Triangle& tri : mesh.triangles)
		{
			glm::mat4 model = ObjectToModel(mesh.transform);

			if (BackfaceCulling(camDir, model, tri))
				continue;

			glm::vec2 a = ModelToNDC(tri.a, model);
			glm::vec2 b = ModelToNDC(tri.b, model);
			glm::vec2 c = ModelToNDC(tri.c, model);

			NDCToPixel(a); 
			NDCToPixel(b); 
			NDCToPixel(c); 

			BoundingBox box = GetTriangleBoundingBox(a, b, c);

			int minPixelX = (int)box.minMaxX.x;
			int maxPixelX = (int)box.minMaxX.y;
			int minPixelY = (int)box.minMaxY.x;
			int maxPixelY = (int)box.minMaxY.y;

			for (int y = minPixelY; y <= maxPixelY; y++)
			{
				for (int x = minPixelX; x <= maxPixelX; x++)
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
	return sideAB && sideBC && sideCA;
}

void Renderer::NDCToPixel(glm::vec2& q)
{
	q = q * 0.5f + 0.5f;
	q = q * screenResolution;
	q = glm::round(q);
}

void Renderer::PixelToNDC(glm::vec2& q)
{
	q = q / screenResolution;
	q = q * 2.0f - 1.0f;
}

glm::vec2 Renderer::ModelToNDC(glm::vec3& point, glm::mat4& model)
{
	float aspectRatio = screenResolution.x / screenResolution.y;

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

glm::mat4 Renderer::ObjectToModel(Transform& objectTransform)
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, objectTransform.location);
	model = glm::rotate(model, glm::radians(objectTransform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(objectTransform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(objectTransform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, objectTransform.scale);

	return model;
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

bool Renderer::BackfaceCulling(glm::vec3& cameraDirection, glm::mat4& model, Triangle& tri)
{
	glm::vec3 worldA = glm::vec3(model * glm::vec4(tri.a, 1.0));
	glm::vec3 worldB = glm::vec3(model * glm::vec4(tri.b, 1.0));
	glm::vec3 worldC = glm::vec3(model * glm::vec4(tri.c, 1.0));
	glm::vec3 normal = glm::normalize(glm::cross(worldB - worldA, worldC - worldA));

	return (glm::dot(normal, cameraDirection) >= 0.0f);
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