#pragma once
#include "Shape.h"

class Cone : public Shape {
	Mesh baseMesh;
public:
	Cone(float xCenter, float yCenter, float zCenter, float radius, float height, unsigned int sectorCount);
	void Draw(Shader& shader);

};