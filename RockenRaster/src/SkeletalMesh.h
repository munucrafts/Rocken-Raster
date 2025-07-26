#pragma once
#include "Primitives.h"


struct SkeletalMesh : public StaticMesh
{
public:
	SkeletalMesh();
	~SkeletalMesh() = default;
	void LoadObjectFile(std::string objPath, std::string texPath) override;
	void LoadAnimationFile(std::string animPath);
	void PlayAnimation();

public:
	bool animate;
};

