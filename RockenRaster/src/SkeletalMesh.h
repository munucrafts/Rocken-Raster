#pragma once
#include "Primitives.h"


struct SkeletalMesh : public StaticMesh
{
public:
	SkeletalMesh();
	~SkeletalMesh() = default;

	void LoadObjectFile(const std::string& objPath, const std::string& texPath) override;
	void LoadAnimationFile(const std::string& animPath);
	void PlayAnimation(const float deltaTime);

public:
	bool animate;
};

