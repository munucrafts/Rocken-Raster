#include <iostream>
#include "Renderer.h"
#include "Scenes.h"
#include "Light.h"

Renderer::Renderer()
{
	firstFrame = true;
	frameCount = 0;
	sceneJustUpdated = true;
	nearClip = 0.01f;
	farClip = 100.0f;
	deltaTime = 0.0f;
	screenResolution = glm::vec2(0.0f);
	projectionType = PERSPECTIVE;
	viewMode = LIT;
	camera.transform.location = glm::vec3(0.0f, 0.0f, 13.0f);
	atmFog = nullptr;
	fogFactor = 0.0f;

	StylizedGuitar* stylizedGuitar = new StylizedGuitar();
	Windmill* windmill = new Windmill();
	Space* space = new Space();
	RetroKeyboard* retroKeyboard = new RetroKeyboard();
	Chestnut* chestnut = new Chestnut();

	allSceneRefs = {stylizedGuitar, windmill, space, retroKeyboard, chestnut};
	allSceneRefs[1]->LoadIntoScene(activeScene);

	//Currently Not Doing Multithreading
	//totalNumThreads = std::thread::hardware_concurrency() - 1;
	//allThreads.resize(totalNumThreads);
}

void Renderer::HandleUI()
{
	float buttonWidth = 150.0f;
	float buttonHeight = 40.0f;
	float spacing = 8.0f;
	int buttonsPerRow = 2;

	ImGui::Begin("Renderer Settings");
	ImGui::Spacing();

	{
		ImGui::Text("  Projection Mode");
		ImGui::Spacing();

		if (ImGui::Button("Perspective", ImVec2(buttonWidth, buttonHeight)))
		{
			projectionType = PERSPECTIVE;
			FlagSceneUpdate();
		}

		ImGui::SameLine();

		if (ImGui::Button("Orthographic", ImVec2(buttonWidth, buttonHeight)))
		{
			projectionType = ORTHOGRAPHIC;
			FlagSceneUpdate();
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	}

	{
		ImGui::Text("  View Mode");
		ImGui::Spacing();

		if (ImGui::Button("LIT", ImVec2(buttonWidth, buttonHeight)))
			viewMode = LIT;

		ImGui::SameLine();

		if (ImGui::Button("UNLIT", ImVec2(buttonWidth, buttonHeight)))
			viewMode = UNLIT;

		if (ImGui::Button("TRIANGULATE", ImVec2(buttonWidth, buttonHeight)))
			viewMode = TRIANGULATE;

		ImGui::SameLine();

		if (ImGui::Button("DEPTH", ImVec2(buttonWidth, buttonHeight)))
			viewMode = DEPTH;

		if (ImGui::Button("NORMAL", ImVec2(buttonWidth, buttonHeight)))
			viewMode = NORMAL;

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	}

	{
		ImGui::Text("  Sky Color");
		ImGui::Spacing();

		float top[4] = { activeScene.sceneSkyColor.topSky.r, activeScene.sceneSkyColor.topSky.g, activeScene.sceneSkyColor.topSky.b, activeScene.sceneSkyColor.topSky.a };
		float bottom[4] = { activeScene.sceneSkyColor.bottomSky.r, activeScene.sceneSkyColor.bottomSky.g, activeScene.sceneSkyColor.bottomSky.b, activeScene.sceneSkyColor.bottomSky.a };

		if (ImGui::ColorEdit4("Top Sky", top))
			activeScene.sceneSkyColor.topSky = glm::vec4(top[0], top[1], top[2], top[3]);

		if (ImGui::ColorEdit4("Bottom Sky", bottom))
			activeScene.sceneSkyColor.bottomSky = glm::vec4(bottom[0], bottom[1], bottom[2], bottom[3]);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	}

	{
		ImGui::Text("  Scenes");
		ImGui::Spacing();

		for (int i = 0; i < allSceneRefs.size(); ++i)
		{
			Scene* scene = allSceneRefs[i];

			if (ImGui::Button(scene->sceneName.c_str(), ImVec2(buttonWidth, buttonHeight)) && scene->sceneName != activeScene.sceneName)
			{
				activeScene.UnloadScene();
				scene->LoadIntoScene(activeScene);
				FlagSceneUpdate();
				atmFog = nullptr;
				fogFactor = 0.0f;
			}

			if ((i + 1) % buttonsPerRow != 0)
				ImGui::SameLine(0.0f, spacing);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	}

	ImGui::End();
}

void Renderer::RenderChunk(Mesh* mesh, Triangle& tri, glm::vec3& pixelA, glm::vec3& pixelB, glm::vec3& pixelC, int minX, int minY, int maxX, int maxY)
{
	for (int y = minY; y <= maxY; y++)
	{
		for (int x = minX; x <= maxX; x++)
		{
			glm::vec3 weights;

			if (InsideTriangle((glm::vec2)pixelA, (glm::vec2)pixelB, (glm::vec2)pixelC, glm::vec2(x, y), weights))
			{
				glm::vec3 abcDepths = glm::vec3(pixelA.z, pixelB.z, pixelC.z);
				float pixelDepth = 1.0f / glm::dot(1.0f / abcDepths, weights);
				int pixelIndex = x + y * (int)screenResolution.x;

				if (pixelIndex < 0 || pixelIndex >= depthBuffer.size())
					continue;

				if (pixelDepth <= depthBuffer[pixelIndex])
				{
					depthBuffer[pixelIndex] = pixelDepth;
					glm::vec2 texCoords;
					glm::vec3 normal;

					if (projectionType == PERSPECTIVE)
					{
						glm::vec2 uv0 = tri.vertices[0].uv / pixelA.z;
						glm::vec2 uv1 = tri.vertices[1].uv / pixelB.z;
						glm::vec2 uv2 = tri.vertices[2].uv / pixelC.z;
						glm::vec2 uvInterp = uv0 * weights.x + uv1 * weights.y + uv2 * weights.z;

						glm::vec3 nor0 = tri.vertices[0].normal / pixelA.z;
						glm::vec3 nor1 = tri.vertices[1].normal / pixelB.z;
						glm::vec3 nor2 = tri.vertices[2].normal / pixelC.z;
						glm::vec3 norInterp = nor0 * weights.x + nor1 * weights.y + nor2 * weights.z;

						float invZ = weights.x / pixelA.z + weights.y / pixelB.z + weights.z / pixelC.z;

						texCoords = uvInterp / invZ;
						normal = norInterp / invZ;
					}
					else if (projectionType == ORTHOGRAPHIC)
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
					glm::vec4 finalColor = GetColorBasedOnViewMode(mesh, tri, texCoords, pixelDepth, normal);

					if (viewMode == LIT)
					{
						for (Entity* light : activeScene.entities)
						{
							if (DirectionalLight* direcLight = dynamic_cast<DirectionalLight*>(light))
							{
								glm::vec3 lightDir = glm::normalize(direcLight->direction);
								accLightIntensity += glm::clamp(glm::dot(normal, -lightDir), 0.0f, 1.0f) * direcLight->intensity;
							}
						}

						if (atmFog)
						{
							fogFactor = atmFog->CalculateFogFactor(nearClip, farClip, pixelDepth);
							finalColor = glm::mix(finalColor, activeScene.sceneSkyColor.topSky, fogFactor);
						}
					}

					DrawPixel(glm::vec2(x, y), accLightIntensity * finalColor);
				}
			}
		}
	}
}

void Renderer::Render(float width, float height, float delta)
{
	if (finalImage)
	{
		if (width != finalImage->GetWidth() || height != finalImage->GetHeight())
		{
			finalImage->Resize(width, height);
			frameBuffer.resize(width * height);
			depthBuffer.resize(width * height);
			screenResolution = glm::vec2(width, height);
			frameCount = 0;
			sceneJustUpdated = true;
		}
	}
	else
	{
		finalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		frameBuffer.resize(width * height);
		depthBuffer.resize(width * height);
		screenResolution = glm::vec2(width, height);
		FlagSceneUpdate();
	}

	if (activeScene.entities.empty())
		return;

	deltaTime = delta / 25.0f;

	ClearBackground();
	ResetDepthBuffer();
	HandleUI();

	camera.NavigateCamera(deltaTime, projectionType);

	for (Entity* entity : activeScene.entities)
	{
		if (entity->audioComp) entity->audioComp->PlayAudio();

		if (entity->mobility == MOVABLE)
		{
			entity->RotateEntity(deltaTime);
			entity->MoveEntity(deltaTime);
			entity->ScaleEntity(deltaTime);

			if (ParticleSystem* ps = dynamic_cast<ParticleSystem*>(entity))
				ps->EmitParticles(deltaTime * 0.01f);
		}

		if (!atmFog) atmFog = dynamic_cast<ExponentialFog*>(entity);

		if (Mesh* mesh = dynamic_cast<Mesh*>(entity))
		{
			glm::mat4 modelWorld;

			if (mesh->mobility == STATIC)
			{
				if (firstFrame || sceneJustUpdated)
				{
					modelWorld = ModelToWorld(mesh->transform);
					mesh->bakedTransform = modelWorld;
				}
				else
				{
					modelWorld = mesh->bakedTransform;
				}
			}
			else if (mesh->mobility == MOVABLE)
			{
				mesh->isMoving = glm::length(mesh->speedComp.linearSpeed) + glm::length(mesh->speedComp.angularSpeed) + glm::length(mesh->speedComp.scalingSpeed) > 0;

				if (firstFrame || mesh->isMoving || sceneJustUpdated)
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
				glm::vec4 clipA = WorldToClip(tri.vertices[0].vert, modelWorld, mesh);
				glm::vec4 clipB = WorldToClip(tri.vertices[1].vert, modelWorld, mesh);
				glm::vec4 clipC = WorldToClip(tri.vertices[2].vert, modelWorld, mesh);

				bool outA = PointOutsideClipSpace(clipA);
				bool outB = PointOutsideClipSpace(clipB);
				bool outC = PointOutsideClipSpace(clipC);

				if (mesh->Retriangulate(outA + outB + outC))
					continue;

				glm::vec3 ndcA = glm::vec3(clipA) / clipA.w;
				glm::vec3 ndcB = glm::vec3(clipB) / clipB.w;
				glm::vec3 ndcC = glm::vec3(clipC) / clipC.w;

				glm::vec3 pixelA = NDCToPixel(ndcA);
				glm::vec3 pixelB = NDCToPixel(ndcB);
				glm::vec3 pixelC = NDCToPixel(ndcC);

				BoundingBox box = GetTriangleBoundingBox(pixelA, pixelB, pixelC);

				int minPixelX = (int)box.minMaxX.x;
				int maxPixelX = (int)box.minMaxX.y;
				int minPixelY = (int)box.minMaxY.x;
				int maxPixelY = (int)box.minMaxY.y;

				RenderChunk(mesh, tri, pixelA, pixelB, pixelC, minPixelX, minPixelY, maxPixelX, maxPixelY);
			}
		}
	}

	finalImage->SetData(frameBuffer.data());

	if (frameCount < 1) frameCount++;
	else
	{
		sceneJustUpdated = false;
		firstFrame = false;
	}
}

void Renderer::FlagSceneUpdate()
{
	frameCount = 0;
	sceneJustUpdated = true;
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

glm::vec3 Renderer::NDCToPixel(glm::vec3& q)
{
	glm::vec3 p = q * 0.5f + 0.5f;
	p = p * glm::vec3(screenResolution, 1.0f);
	return p;
}

glm::vec4 Renderer::WorldToClip(glm::vec3& point, glm::mat4& model, Mesh* currentMesh)
{
	static glm::mat4 cachedViewProj;

	bool recomputeViewProj = camera.isMoving || sceneJustUpdated || firstFrame;

	if (recomputeViewProj)
	{
		float aspectRatio = (float)screenResolution.x / (float)screenResolution.y;
		glm::mat4 view = camera.GetViewMatrix(firstFrame);
		glm::mat4 proj;

		switch (projectionType)
		{
		case ORTHOGRAPHIC:
			proj = glm::ortho(-camera.orthoValue * aspectRatio, camera.orthoValue * aspectRatio, -camera.orthoValue, camera.orthoValue, -50.0f, 50.0f);
			break;
		case PERSPECTIVE:
			proj = glm::perspective(camera.fov, aspectRatio, nearClip, farClip);
			break;
		}

		cachedViewProj = proj * view;
	}

	if (currentMesh->isMoving || (currentMesh->mobility == STATIC && recomputeViewProj))
	{
		currentMesh->bakedProjection = cachedViewProj * model;
	}

	glm::vec4 clip = currentMesh->bakedProjection * glm::vec4(point, 1.0f);
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

void Renderer::ClearBackground()
{
	for (int y = 0; y < screenResolution.y; y++)
	{
		float gradient = float(y) / (screenResolution.y - 1);
		glm::vec4 rowColor = glm::mix(activeScene.sceneSkyColor.bottomSky, activeScene.sceneSkyColor.topSky, gradient);

		for (int x = 0; x < screenResolution.x; x++)
		{
			int index = x + y * screenResolution.x;
			frameBuffer[index] = ColorToRGBA(rowColor);
		}
	}
}

void Renderer::DrawPixel(glm::vec2& pixelLoc, glm::vec4& color)
{
	int x = (int)pixelLoc.x;
	int y = (int)pixelLoc.y;

	if (x >= 0 && x < (int)screenResolution.x && y >= 0 && y < (int)screenResolution.y)
		frameBuffer[x + y * (int)screenResolution.x] = ColorToRGBA(color);
}

glm::vec4 Renderer::GetColorBasedOnViewMode(Mesh* mesh, Triangle& tri, glm::vec2& texCoords, float depthAtPixel, glm::vec3& interpNormal)
{
	switch (viewMode)
	{
		case LIT:
			return mesh->mat.texture.SampleTexture(texCoords);
		case UNLIT:
			return mesh->mat.texture.SampleTexture(texCoords);
		case TRIANGULATE:
		{
			size_t memoryHash = reinterpret_cast<size_t>(&tri) * 9973;

			glm::vec3 color = glm::vec3(pow(((memoryHash >> 0) & 0xFF) / 255.0f, 1.0f), 
										pow(((memoryHash >> 8) & 0xFF) / 255.0f, 1.0f), 
										pow(((memoryHash >> 16) & 0xFF) / 255.0f, 1.0f));

			int maxIndex = (color.r > color.g && color.r > color.b) ? 0 : (color.g > color.b) ? 1 : 2;
			color[maxIndex] = 1.0f;
			return glm::vec4(color, 1.0f);
		}
		case DEPTH: 
		{
			return glm::vec4(depthAtPixel);
		}
		case NORMAL: 
		{
			return glm::vec4(interpNormal * 0.5f + 0.5f, 1.0f);
		}
		default:
			return glm::vec4(0.0f);
	}
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

	return { glm::vec2(minX, maxX), glm::vec2(minY, maxY)};
}

std::shared_ptr<Walnut::Image>& Renderer::GetFinalImage()
{
	return finalImage;
}