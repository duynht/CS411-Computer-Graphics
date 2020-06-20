#pragma once
#include "Shape.h"

class CircularParaboloid : public Shape {
	Mesh baseMesh;
	unsigned int primitiveRestartIndex;
public:
	CircularParaboloid(float xCenter, float yCenter, float zCenter, float skirtRadius, float height, unsigned int sectorCount, unsigned int stackCount);
	void Draw(Shader& shader);
};