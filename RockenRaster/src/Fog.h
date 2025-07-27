#pragma once
#include "Primitives.h"

class Fog : public Entity
{
public:
	Fog() = default;
	virtual ~Fog() = default;

public:
	float fogDensity = 0.0f;
};

class ExponentialFog : public Fog
{
public:
	ExponentialFog();
	virtual ~ExponentialFog() = default;
	float CalculateFogFactor(const float nearClip, const float farClip, const float pixelDepth);

public:
	float falloffDistance = 0.0f;
};