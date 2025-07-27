#include "SkeletalMesh.h"

SkeletalMesh::SkeletalMesh()
{
	mobility = MOVABLE;
	animate = false;
}

void SkeletalMesh::LoadObjectFile(const std::string& objPath, const std::string& texPath)
{
}

void SkeletalMesh::LoadAnimationFile(const std::string& animPath)
{
}

void SkeletalMesh::PlayAnimation(const float deltaTime)
{
}
