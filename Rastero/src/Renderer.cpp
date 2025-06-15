#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
{
	deltaTime = 0.0f;
	screenResolution = glm::vec2(0.0f);
	projection = PERSPECTIVE;

	Mesh testMesh;
	testMesh.LoadObjectFile("../Assets/Monkey.obj");
	scene.meshes.push_back(testMesh);
}

void Renderer::Render(float width, float height, float delta)
{
	if (image)
	{
		if (width != image->GetWidth() || height != image->GetHeight())
		{
			image->Resize(width, height);
			imageData.resize(width * height);
			depthBuffer.resize(width * height);
			screenResolution = glm::vec2(width, height);
		}
	}
	else
	{
		image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		imageData.resize(width * height);
		depthBuffer.resize(width * height);
		screenResolution = glm::vec2(width, height);
	}

	deltaTime = delta / 10.0f;
	ClearBackground(glm::vec4(0.2f));
	ResetDepthBuffer();

	if (scene.meshes.empty())
		return;

	// Coordinate Convention = GLM / OpenGL Convention (Right Handed Convention)
	// Left -> Right = X, Bottom -> Top = Y, Camera -> Screen = -Z
	// Triangle Winding Convention = counter-clockwise (CCW)
	// Note - Model = Object's Local Transform Transfomred to World Space Using Orthographic or Perspective Projection
	// NDC = Normalized Device Coordinates = Model's World Transform Transformed to UV Space { (0, 0) to (1, 1) } and then to NDC Space { (-1, -1) to (1, 1) }
	// Pixel = Basically Screen Space which is { (0, 0) to (screenResolution.x, screenResolution.y) }

	Camera cam;
	cam.transform.location = glm::vec3(0.0f, 0.0f, 10.0f);   
	cam.forward = glm::vec3(0.0f, 0.0f, -1.0f);             
	cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cam.right = glm::vec3(1.0f, 0.0f, 0.0f);
	cam.fov = glm::radians(45.0f);

	for (Mesh& mesh : scene.meshes)
	{
		mesh.transform.scale = glm::vec3(1.0f);
		mesh.transform.location = glm::vec3(0.0f, 0.0f, -7.0f);
		mesh.AddRotation(glm::vec3(0.0f, 0.3f, 0.0f), deltaTime);

		for (Triangle& tri : mesh.triangles)
		{
			glm::mat4 model = ObjectToModel(mesh.transform);

			glm::vec3 a = ModelToNDC(tri.a, model, cam);
			glm::vec3 b = ModelToNDC(tri.b, model, cam);
			glm::vec3 c = ModelToNDC(tri.c, model, cam);
			
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
					glm::vec3 weights;

					if (InsideTriangle(a, b, c, glm::vec2(x, y), weights))
					{
						glm::vec3 abcDepths = glm::vec3(a.z, b.z, c.z);
						float pixelDepth = glm::dot(abcDepths, weights);
						int pixelIndex = x + y * screenResolution.x;

						if (pixelDepth >= depthBuffer[pixelIndex]) 
						{
							depthBuffer[pixelIndex] = pixelDepth;
							DrawPixel(glm::vec2(x, y), tri.color);
						}
					}
				}
			}
		}
	}

	image->SetData(imageData.data());
}

uint32_t Renderer::ColorToRGBA(glm::vec4& color)
{
	glm::vec4 clampedColor = 255.0f * glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
	uint8_t r = (uint8_t)clampedColor.x;
	uint8_t g = (uint8_t)clampedColor.y;
	uint8_t b = (uint8_t)clampedColor.z;
	uint8_t a = (uint8_t)clampedColor.a;
	uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
	return result;
}

float Renderer::GetSignedTriangleArea(glm::vec2& a, glm::vec2& b, glm::vec2& p)
{
	glm::vec2 ap = p - a;
	glm::vec2 ab = b - a;
	glm::vec2 abPerp = glm::vec2(ab.y, -ab.x);
	return glm::dot(ap, abPerp) / 2.0f;
}

bool Renderer::InsideTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p, glm::vec3& weights)
{
	float areaABP = GetSignedTriangleArea(a, b, p);
	float areaBCP = GetSignedTriangleArea(b, c, p);
	float areaCAP = GetSignedTriangleArea(c, a, p);

	float sumArea = areaABP + areaBCP + areaCAP;

	float weightA = areaBCP / sumArea;
	float weightB = areaCAP / sumArea;
	float weightC = areaABP / sumArea;

	weights = glm::vec3(weightA, weightB, weightC);

	return areaABP >= 0.0f && areaBCP >= 0.0f && areaCAP >= 0.0f && sumArea > 0.0f;
}

void Renderer::NDCToPixel(glm::vec3& q)
{
	q = q * 0.5f + 0.5f;
	q = q * glm::vec3(screenResolution, 1.0f);
}

void Renderer::PixelToNDC(glm::vec2& q)
{
	q = q / screenResolution;
	q = q * 2.0f - 1.0f;
}

glm::vec3 Renderer::ModelToNDC(glm::vec3& point, glm::mat4& model, Camera& cam)
{
	float aspectRatio = (float)screenResolution.x / screenResolution.y;

	switch (projection)
	{
		case ORTHOGRAPHIC:
		{
			glm::mat4 orthoMat = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -10.0f, 10.0f);
			glm::mat4 transform = orthoMat * model;
			glm::vec4 transformed = transform * glm::vec4(point, 1.0f);
			return glm::vec3(transformed);
		}
		case PERSPECTIVE:
		{
			glm::mat4 persMat = glm::perspective(cam.fov, aspectRatio, 0.1f, 100.0f);
			glm::mat4 transform = persMat * model;
			glm::vec4 transformed = transform * glm::vec4(point, 1.0f);
			transformed /= transformed.w;
			return glm::vec3(transformed);
		}
	}
}

glm::mat4 Renderer::ObjectToModel(Transform& objectTransform)
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::scale(model, objectTransform.scale);
	model = glm::rotate(model, glm::radians(objectTransform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(objectTransform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(objectTransform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(glm::mat4(1.0f), objectTransform.location) * model;

	return model;
}

void Renderer::ClearBackground(glm::vec4& bgColor)
{
	std::fill(imageData.begin(), imageData.end(), ColorToRGBA(bgColor));
}

void Renderer::DrawPixel(glm::vec2 pixelLoc, glm::vec4 pixelColor)
{
	int x = (int)pixelLoc.x;
	int y = (int)pixelLoc.y;

	if (x >= 0 && x < (int)screenResolution.x && y >= 0 && y < (int)screenResolution.y)
		imageData[x + y * (int)screenResolution.x] = ColorToRGBA(pixelColor);
}

void Renderer::ResetDepthBuffer()
{
	std::fill(depthBuffer.begin(), depthBuffer.end(), -FLT_MAX);
}

BoundingBox Renderer::GetTriangleBoundingBox(glm::vec3& a, glm::vec3& b, glm::vec3& c)
{
	int minX = std::max(0, (int)std::floor(std::min({ a.x, b.x, c.x })));
	int maxX = std::min((int)screenResolution.x, (int)std::ceil(std::max({ a.x, b.x, c.x })));

	int minY = std::max(0, (int)std::floor(std::min({ a.y, b.y, c.y })));
	int maxY = std::min((int)screenResolution.y, (int)std::ceil(std::max({ a.y, b.y, c.y })));

	int minZ = (int)std::floor(std::min({ a.z, b.z, c.z }));
	int maxZ = (int)std::ceil(std::max({ a.z, b.z, c.z }));

	return { glm::vec2(minX, maxX), glm::vec2(minY, maxY), glm::vec2(minZ, maxZ)};
}

std::shared_ptr<Walnut::Image>& Renderer::GetImage()
{
	return image;
}