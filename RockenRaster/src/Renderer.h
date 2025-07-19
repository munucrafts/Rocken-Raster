#pragma once
#include <Walnut/Image.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mutex>
#include <thread>
#include "Primitives.h"
#include "Camera.h"
#include "Fog.h"

class Renderer
{
public:
	Renderer();
	void Render(float width, float height, float delta);
	std::shared_ptr<Walnut::Image>& GetFinalImage();

private:
	uint32_t ColorToRGBA(glm::vec4& color);
	float GetSignedTriangleArea(glm::vec2& a, glm::vec2& b, glm::vec2& p);
	bool InsideTriangle(glm::vec2& a, glm::vec2& b, glm::vec2& c, glm::vec2& p, glm::vec3& weights);
	glm::vec3 NDCToPixel(glm::vec3& q);
	glm::vec4 WorldToClip(glm::vec3& point, glm::mat4& model, Mesh* currentMesh);
	bool PointOutsideClipSpace(glm::vec4& point);
	glm::mat4 ModelToWorld(Transform& objectTransform);
	void ClearBackground();
	BoundingBox GetTriangleBoundingBox(glm::vec3& a, glm::vec3& b, glm::vec3& c);
	void ResetDepthBuffer();
	void DrawPixel(glm::vec2& pixelLoc, glm::vec4& color);
	glm::vec4 GetColorBasedOnViewMode(Mesh* mesh, Triangle& tri, glm::vec2& texCoords, float depthAtPixel, glm::vec3& interpNormal);
	void HandleUI();
	void RenderChunk(int threadId);

private:
	std::vector<uint32_t> frameBuffer;
	std::vector<float> depthBuffer;
	std::shared_ptr<Walnut::Image> finalImage;
	glm::vec2 screenResolution;
	Scene activeScene;
	std::vector<Scene*> allSceneRefs;
	Camera camera;
	Projection projectionType;
	ViewMode viewMode;
	float deltaTime;
	float nearClip;
	float farClip;
	bool firstFrame;
	int frameCount;
	bool sceneJustUpdated;
	ExponentialFog* atmFog;
	float fogFactor;
	std::mutex mtx;
	std::vector<std::thread> allThreads;
	int totalNumThreads;
};

