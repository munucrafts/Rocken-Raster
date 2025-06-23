#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
{
	nearClip = 0.01f;
	farClip = 100.0f;
	skyColor = glm::vec4(0.53f, 0.81f, 0.98f, 1.0f);
	deltaTime = 0.0f;
	screenResolution = glm::vec2(0.0f);
	projection = PERSPECTIVE;

	{
		Mesh Windmill;
		Windmill.LoadObjectFile("../Assets/Windmill.obj", "../Assets/Windmill.png");
		Windmill.transform.scale = glm::vec3(2.5f);
		Windmill.transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		Windmill.transform.location = glm::vec3(0.0f, -2.5f, -10.0f);
		scene.meshes.push_back(Windmill);

		Mesh Propeller;
		Propeller.LoadObjectFile("../Assets/Propeller.obj", "../Assets/Propeller.png");
		Propeller.transform.scale = glm::vec3(2.5f);
		Propeller.transform.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
		Propeller.transform.location = glm::vec3(0.0f, 1.37f, -9.97f);
		Propeller.speedComp.angularSpeed = glm::vec3(0.0f, 0.0f, -0.2f);
		scene.meshes.push_back(Propeller);

		Light atmLight;
		atmLight.location = {glm::vec3(-20.0f, 20.f, -10.f)};
		atmLight.direction = { glm::vec3(-20.0f, 20.f, -10.f) };
		atmLight.intensityMultiplier = 2.0f;
		scene.lights.push_back(atmLight);
	}
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
	ClearBackground(skyColor);
	ResetDepthBuffer();

	if (scene.meshes.empty())
		return;

	// Coordinate Convention = GLM / OpenGL Convention (Right Handed Convention)
	// Left -> Right = X, Bottom -> Top = Y, Camera -> Screen = -Z
	// Triangle Winding Convention = Counter Clockwise (CCW)
	// Note - Model = Object's Local Transform 
	// World = Model Transfomred to World Space Using Orthographic or Perspective Projection Using "Model Matrix"
	// NDC = Normalized Device Coordinates = World -> View -> Clip -> UV Space { (0, 0) to (1, 1) } -> NDC Space { (-1, -1) to (1, 1) }
	// Pixel / Screen = Basically Screen Space which is { (0, 0) to (screenResolution.x, screenResolution.y) }

	camera.NavigateCamera(deltaTime, projection);

	for (Mesh& mesh : scene.meshes)
	{
		mesh.RotateMesh(deltaTime);

		for (Triangle& tri : mesh.triangles)
		{
			glm::mat4 modelWorld = ModelToWorld(mesh.transform);

			glm::vec4 clipA = WorldToClip(tri.vertices[0].vert, modelWorld);
			glm::vec4 clipB = WorldToClip(tri.vertices[1].vert, modelWorld);
			glm::vec4 clipC = WorldToClip(tri.vertices[2].vert, modelWorld);

			bool outA = PointOutsideClipSpace(clipA);
			bool outB = PointOutsideClipSpace(clipB);
			bool outC = PointOutsideClipSpace(clipC);

			int clipCount = outA + outB + outC;
			
			if (clipCount > 0) continue;

			glm::vec3 ndcA = (glm::vec3)clipA / clipA.w;
			glm::vec3 ndcB = (glm::vec3)clipB / clipB.w;
			glm::vec3 ndcC = (glm::vec3)clipC / clipC.w;
			
			NDCToPixel(ndcA); 
			NDCToPixel(ndcB); 
			NDCToPixel(ndcC); 
			
			BoundingBox box = GetTriangleBoundingBox(ndcA, ndcB, ndcC);

			int minPixelX = (int)box.minMaxX.x;
			int maxPixelX = (int)box.minMaxX.y;
			int minPixelY = (int)box.minMaxY.x;
			int maxPixelY = (int)box.minMaxY.y;
			
			for (int y = minPixelY; y <= maxPixelY; y++)
			{
				for (int x = minPixelX; x <= maxPixelX; x++)
				{
					glm::vec3 weights;

					if (InsideTriangle((glm::vec2)ndcA, (glm::vec2)ndcB, (glm::vec2)ndcC, glm::vec2(x, y), weights))
					{
						glm::vec3 abcDepths = glm::vec3(ndcA.z, ndcB.z, ndcC.z);
						float pixelDepth = 1.0f / (glm::dot(1.0f / abcDepths, weights));
						int pixelIndex = x + y * screenResolution.x;
						
						if (pixelIndex < 0 || pixelIndex >= depthBuffer.size()) 
							continue;					

						if (pixelDepth <= depthBuffer[pixelIndex])
						{
							depthBuffer[pixelIndex] = pixelDepth;
							glm::vec2 texCoords;
							glm::vec3 normal;

							if (projection == PERSPECTIVE)
							{
								glm::vec2 uv0 = tri.vertices[0].uv / ndcA.z;
								glm::vec2 uv1 = tri.vertices[1].uv / ndcB.z;
								glm::vec2 uv2 = tri.vertices[2].uv / ndcC.z;
								glm::vec2 uvInterp = uv0 * weights.x + uv1 * weights.y + uv2 * weights.z;
								
								glm::vec3 nor0 = tri.vertices[0].normal / ndcA.z;
								glm::vec3 nor1 = tri.vertices[1].normal / ndcB.z;
								glm::vec3 nor2 = tri.vertices[2].normal / ndcC.z;
								glm::vec3 norInterp = nor0 * weights.x + nor1 * weights.y + nor2 * weights.z;

								float invZ = weights.x / ndcA.z + weights.y / ndcB.z + weights.z / ndcC.z;

								texCoords = uvInterp / invZ;
								normal = norInterp / invZ;
							}
							else if (projection == ORTHOGRAPHIC)
							{
								texCoords = tri.vertices[0].uv * weights.x +
											tri.vertices[1].uv * weights.y +
											tri.vertices[2].uv * weights.z;

								normal = tri.vertices[0].normal * weights.x +
										 tri.vertices[1].normal * weights.y +
										 tri.vertices[2].normal * weights.z;
							}

							normal = glm::normalize(normal);
							float intensity = 1.0f;

							for (Light& light : scene.lights)
							{
								if (glm::distance(light.location, mesh.transform.location) > farClip) continue;

								glm::vec3 lightDir = glm::normalize(light.direction);
								intensity += glm::clamp(glm::dot(normal, -lightDir), 0.0f, 1.0f) * light.intensityMultiplier;
							}
			
							glm::vec4 texCol = mesh.mat.texture.LoadColorAtTexureCoordinates(texCoords);
							DrawPixel(glm::vec2(x, y), intensity * texCol);
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
	glm::vec2 ab = b - a;
	glm::vec2 ap = p - a;
	glm::vec2 abPerp = glm::vec2(-ab.y, ab.x);
	return glm::dot(ap, abPerp) / 2.0f;
}

bool Renderer::InsideTriangle(glm::vec2& a, glm::vec2& b, glm::vec2& c, glm::vec2& p, glm::vec3& weights)
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

glm::vec4 Renderer::WorldToClip(glm::vec3& point, glm::mat4& model)
{
	float aspectRatio = (float)screenResolution.x / (float)screenResolution.y;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 transform;
	glm::mat4 proj;
	glm::vec4 clip;

	switch (projection)
	{
		case ORTHOGRAPHIC:
			proj = glm::ortho(-camera.orthoValue * aspectRatio, camera.orthoValue * aspectRatio,
							  -camera.orthoValue, camera.orthoValue, -50.0f, 50.0f);
			break;
		case PERSPECTIVE:
			proj = glm::perspective(camera.fov, aspectRatio, nearClip, farClip);
			break;
	}

	transform = proj * view * model;
	clip = transform * glm::vec4(point, 1.0f);
	return clip;
}

bool Renderer::PointOutsideClipSpace(glm::vec4& point)
{
	return ( point.x < -point.w || point.x > point.w ||
			 point.y < -point.w || point.y > point.w ||
			 point.z < -point.w || point.z > point.w );
}

glm::mat4 Renderer::ModelToWorld(Transform& objectTransform)
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

void Renderer::DrawPixel(glm::vec2& pixelLoc, glm::vec4& color)
{
	int x = (int)pixelLoc.x;
	int y = (int)pixelLoc.y;

	if (x >= 0 && x < (int)screenResolution.x && y >= 0 && y < (int)screenResolution.y)
		imageData[x + y * (int)screenResolution.x] = ColorToRGBA(color);
}

void Renderer::ResetDepthBuffer()
{
	std::fill(depthBuffer.begin(), depthBuffer.end(), FLT_MAX);
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