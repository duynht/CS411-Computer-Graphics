#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <numeric>
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"

class Shape {
protected:
	float xCenter;
	float yCenter;
	float zCenter;

	Mesh mesh;
	Transform motion;

	std::vector<float> GetUnitCircleVertices(unsigned int sectorCount);
public:
	void virtual Draw(Shader& shader) = 0;
	void Rotate(glm::vec3 radians, glm::vec3 axis = glm::vec3());
	void Clean(Shader& shader);
};
