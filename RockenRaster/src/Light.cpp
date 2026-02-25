#include "Light.h"

Light::Light()
{
	intensity = 1.0f;
	direction = glm::vec3(0.0f);
	entType = LIGHT;
 }

DirectionalLight::DirectionalLight()
{
	intensity = 1.0f;
	direction = glm::vec3(0.0f);
	entType = DIRECTIONAL_LIGHT; 
}
