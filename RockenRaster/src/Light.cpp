#include "Light.h"

Light::Light()
{
	intensity = 1.0f;
 }

DirectionalLight::DirectionalLight()
{
	intensity = 1.0f;
	direction = glm::vec3(0.0f);
}
