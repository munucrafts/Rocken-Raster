#pragma once
#include "Primitives.h"

class Fog : public Entity
{
public:
	Fog();
	virtual ~Fog() = default;
	float CalculateFogFactor(float nearClip, float farClip, float pixelDepth);

public:
	float fogDensity;
	float falloffDistance;
};

