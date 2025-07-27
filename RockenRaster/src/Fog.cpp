#include "Fog.h"

ExponentialFog::ExponentialFog()
{
	falloffDistance = 50.0f;
	fogDensity = 0.001f;
	mobility = STATIC;
}

float ExponentialFog::CalculateFogFactor(const float nearClip, const float farClip, const float pixelDepth)
{
	float linearDepth = (2.0f * nearClip * farClip) / (farClip + nearClip - pixelDepth * (farClip - nearClip));
	float fogCoord = glm::clamp((linearDepth - falloffDistance) / (farClip - falloffDistance), 0.0f, 1.0f);
	float fogFactor = 1.0f - glm::exp(-fogCoord * fogDensity);
	fogFactor = glm::clamp(fogFactor, 0.0f, 1.0f);
	return fogFactor * 1000.f;
}

