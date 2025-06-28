#include <iostream>
#include "Renderer.h"
#include "Scenes.h"
#include "Light.h"


Renderer::Renderer()
{
	firstFrame = true;
	nearClip = 0.01f;
	farClip = 100.0f;
	deltaTime = 0.0f;
	screenResolution = glm::vec2(0.0f);
	projection = PERSPECTIVE;
	camera.transform.location = glm::vec3(0.0f, 0.0f, 13.0f);

	Windmill windmill(scene);
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

	glm::vec4 skyTop = glm::vec4(0.22f, 0.71f, 1.0f, 1.0f);
	glm::vec4 skyBottom = glm::vec4(0.83f, 0.84f, 0.85f, 1.0f);

	ClearBackground(skyTop, skyBottom);
	ResetDepthBuffer();

	if (scene.entities.empty())
		return;

	// Coordinate Convention = GLM / OpenGL Convention (Right Handed Convention)
	// Left -> Right = X, Bottom -> Top = Y, Camera -> Screen = -Z
	// Triangle Winding Convention = Counter Clockwise (CCW)
	// Note - Model = Object's Local Transform 
	// World = Model Transfomred to World Space Using Orthographic or Perspective Projection Using "Model Matrix"
	// NDC = Normalized Device Coordinates = World -> View -> Clip -> UV Space { (0, 0) to (1, 1) } -> NDC Space { (-1, -1) to (1, 1) }
	// Pixel / Screen = Basically Screen Space which is { (0, 0) to (screenResolution.x, screenResolution.y) }

	camera.NavigateCamera(deltaTime, projection);

	static Fog* atmFog = nullptr;
	static float fogFactor = 0.0f;

	for (Entity* entity : scene.entities)
	{
		if (entity->mobility == Movable)
		{
			entity->RotateEntity(deltaTime);

			if (ParticleSystem* ps = dynamic_cast<ParticleSystem*>(entity))
				ps->EmitParticles(deltaTime * 0.01f);
		}

		if (!atmFog)
			atmFog = dynamic_cast<Fog*>(entity);

		if (Mesh* mesh = dynamic_cast<Mesh*>(entity))
		{
			glm::mat4 modelWorld;

			if (mesh->mobility == Static)
			{
				if (firstFrame)
				{
					modelWorld = ModelToWorld(mesh->transform);
					mesh->bakedTransform = modelWorld;
				}
				else
				{
					modelWorld = mesh->bakedTransform;
				}
			}
			else if (mesh->mobility == Movable)
			{
				if (firstFrame || mesh->isMoving)
				{
					modelWorld = ModelToWorld(mesh->transform);
					mesh->bakedTransform = modelWorld;
				}
				else
				{
					modelWorld = mesh->bakedTransform;
				}
			}

			for (Triangle& tri : mesh->triangles)
			{
				glm::vec4 clipA = WorldToClip(tri.vertices[0].vert, modelWorld);
				glm::vec4 clipB = WorldToClip(tri.vertices[1].vert, modelWorld);
				glm::vec4 clipC = WorldToClip(tri.vertices[2].vert, modelWorld);

				bool outA = PointOutsideClipSpace(clipA);
				bool outB = PointOutsideClipSpace(clipB);
				bool outC = PointOutsideClipSpace(clipC);

				if (mesh->Retriangulate(outA + outB + outC))
					continue;

				glm::vec3 ndcA = glm::vec3(clipA) / clipA.w;
				glm::vec3 ndcB = glm::vec3(clipB) / clipB.w;
				glm::vec3 ndcC = glm::vec3(clipC) / clipC.w;

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
							float pixelDepth = 1.0f / glm::dot(1.0f / abcDepths, weights);
							int pixelIndex = x + y * (int)screenResolution.x;

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
								float accLightIntensity = 1.0f;

								for (Entity* light : scene.entities)
								{
									if (DirectionalLight* direcLight = dynamic_cast<DirectionalLight*>(light))
									{
										glm::vec3 lightDir = glm::normalize(direcLight->direction);
										accLightIntensity += glm::clamp(glm::dot(normal, -lightDir), 0.0f, 1.0f) * direcLight->intensity;
									}
								}

								glm::vec4 finalColor = mesh->mat.tex ? mesh->mat.texture.LoadColorAtTexureCoordinates(texCoords) : mesh->mat.color;

								if (atmFog)
								{
									fogFactor = atmFog->CalculateFogFactor(nearClip, farClip, pixelDepth);
									finalColor = glm::mix(finalColor, skyTop, fogFactor);
								}
								
								DrawPixel(glm::vec2(x, y), accLightIntensity * finalColor);
							}
						}
					}
				}
			}
			for (Triangle& tri : mesh->newTriangles)
			{
				glm::vec3 ndcA = tri.vertices[0].vert;
				glm::vec3 ndcB = tri.vertices[1].vert;
				glm::vec3 ndcC = tri.vertices[2].vert;

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
							float pixelDepth = 1.0f / glm::dot(1.0f / abcDepths, weights);
							int pixelIndex = x + y * (int)screenResolution.x;

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
								float accLightIntensity = 1.0f;

								for (Entity* light : scene.entities)
								{
									if (DirectionalLight* direcLight = dynamic_cast<DirectionalLight*>(light))
									{
										glm::vec3 lightDir = glm::normalize(direcLight->direction);
										accLightIntensity += glm::clamp(glm::dot(normal, -lightDir), 0.0f, 1.0f) * direcLight->intensity;
									}
								}

								glm::vec4 finalColor = mesh->mat.tex ? mesh->mat.texture.LoadColorAtTexureCoordinates(texCoords) : mesh->mat.color;

								if (atmFog)
								{
									fogFactor = atmFog->CalculateFogFactor(nearClip, farClip, pixelDepth);
									finalColor = glm::mix(finalColor, skyTop, fogFactor);
								}

								DrawPixel(glm::vec2(x, y), accLightIntensity * finalColor);
							}
						}
					}
				}
			}
			mesh->newTriangles.clear();
		}
	}

	image->SetData(imageData.data());
	firstFrame = false;
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
	static glm::mat4 cachedViewProj;
	static glm::vec2 cachedResolution;
	static Projection cachedProjectionType;
	static bool firstRun = true;

	glm::vec4 clip;

	if (camera.isMoving || projection != cachedProjectionType || screenResolution != cachedResolution || firstRun)
	{
		float aspectRatio = (float)screenResolution.x / (float)screenResolution.y;
		glm::mat4 view = camera.GetViewMatrix(firstFrame);
		glm::mat4 proj;

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

		cachedViewProj = proj * view;
		cachedProjectionType = projection;
		cachedResolution = screenResolution;
		firstRun = false;
	}

	clip = cachedViewProj * model * glm::vec4(point, 1.0f);
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

void Renderer::ClearBackground(glm::vec4& topColor, glm::vec4& bottomColor)
{
	for (int y = 0; y < screenResolution.y; y++)
	{
		float gradient = float(y) / (screenResolution.y - 1);
		glm::vec4 rowColor = glm::mix(bottomColor, topColor, gradient);

		for (int x = 0; x < screenResolution.x; x++)
		{
			int index = x + y * screenResolution.x;
			imageData[index] = ColorToRGBA(rowColor);
		}
	}
}

void Renderer::DrawPixel(glm::vec2& pixelLoc, glm::vec4& color)
{
	int x = (int)pixelLoc.x;
	int y = (int)pixelLoc.y;

	if (x >= 0 && x < (int)screenResolution.x && y >= 0 && y < (int)screenResolution.y)
		imageData[x + y * (int)screenResolution.x] = ColorToRGBA(color);
}

bool Renderer::TransformUpdateRequired(Camera& camera)
{
	return camera.isMoving || firstFrame;
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