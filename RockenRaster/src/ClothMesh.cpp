#include "ClothMesh.h"

ClothMesh::ClothMesh()
{
	force = glm::vec3(1.0f);
	drag = 0.01f;
	elasticity = 10.0f;
}

void ClothMesh::SimulateCloth(const float deltaTime)
{
    glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    glm::vec3 windForce = force;
    float windStrength = 5.0f;

    static glm::vec prevPosition;

    for (Triangle& tri : triangles)
    {
        for (Vertex& v : tri.vertices)
        {
            glm::vec3 velocity = (v.vert - v.prevPosition) * (1.0f - drag);
            v.prevPosition = v.vert;

            glm::vec3 totalForce = gravity + windForce * windStrength;

            v.vert += velocity + totalForce * deltaTime * deltaTime;
        }
    }

    for (int i = 0; i < 5; ++i) 
    {
        for (Stick& stick : sticks)
        {
            glm::vec3 delta = stick.pointB - stick.pointA;
            float currentLength = glm::length(delta);
            if (currentLength == 0.0f) continue;

            float diff = (currentLength - stick.restLength) / currentLength;
            glm::vec3 correction = delta * 0.5f * diff * elasticity;

            stick.pointA += correction;
            stick.pointB -= correction;
        }
    }
}

void ClothMesh::InitCloth()
{
	for (Triangle& tri : triangles)
	{
		Stick stickAB, stickBC, stickCA;

		stickAB.pointA = tri.vertices[0].vert;
		stickAB.pointB = tri.vertices[1].vert;

		stickBC.pointA = tri.vertices[1].vert;
		stickBC.pointB = tri.vertices[2].vert;

		stickCA.pointA = tri.vertices[2].vert;
		stickCA.pointB = tri.vertices[0].vert;

		sticks.push_back(stickAB);
		sticks.push_back(stickBC);
		sticks.push_back(stickCA);
	}
}
