#pragma once
#include "Primitives.h"

class Fog : public Entity
{
public:
	Fog() = default;
	virtual ~Fog() = default;

public:
	float fogDensity;
};

class ExponentialFog : public Fog
{
public:
	ExponentialFog();
	virtual ~ExponentialFog() = default;
	float CalculateFogFactor(float nearClip, float farClip, float pixelDepth);

public:
	float falloffDistance;
};