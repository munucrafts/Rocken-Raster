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
	void Render(const float width, const float height, const float delta);
	const std::shared_ptr<Walnut::Image>& GetFinalImage() const;
	void InitRenderer();

private:
	uint32_t ColorToRGBA(const glm::vec4& color) const;
	float GetSignedTriangleArea(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p) const;
	bool InsideTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& p, glm::vec3& weights) const;
	glm::vec3 NDCToPixel(const glm::vec3& q) const;
	glm::vec4 WorldToClip(const glm::vec3& point, const glm::mat4& model, StaticMesh* currentMesh);
	bool PointOutsideClipSpace(const glm::vec4& point) const;
	glm::mat4 ModelToWorld(const Transform& objectTransform) const;
	void ClearBackground();
	BoundingBox GetTriangleBoundingBox(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) const;
	void ResetDepthBuffer();
	void DrawPixel(const glm::vec2& pixelLoc, const glm::vec4& color);
	glm::vec4 GetColorBasedOnViewMode(const StaticMesh* mesh, const Triangle& tri, const glm::vec2& texCoords, const float depthAtPixel, const glm::vec3& interpNormal) const;
	void HandleUI();
	void FlagSceneUpdate();

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
};

