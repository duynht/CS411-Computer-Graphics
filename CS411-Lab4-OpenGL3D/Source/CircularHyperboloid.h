#pragma once
#include "Shape.h"

class CircularHyperboloid : public Shape {
	Mesh topMesh;
	Mesh baseMesh;
	unsigned int primitiveRestartIndex;
public:
	CircularHyperboloid(float xCenter, float yCenter, float zCenter, float hipRadius, float hyperbolaFocus, unsigned int sectorCount, unsigned int stackCount);
	void Draw(Shader& shader);
};