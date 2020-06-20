#pragma once
#include "Shape.h"
#include "Cylinder.h"

class Torus : public Shape {
	unsigned int primitiveRestartIndex;
public:
	Torus(float xCenter, float yCenter, float zCenter, float mainRadius, float tubeRadius, unsigned int mainSectorCount, unsigned int tubeSectorCount);
	void Draw(Shader& shader);
};
