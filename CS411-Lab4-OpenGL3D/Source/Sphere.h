#pragma once
#include "Shape.h"

class Sphere : public Shape {

public:
	Sphere(float xCenter, float yCenter, float zCenter, float radius, unsigned int sectorCount, unsigned int stackCount);
	void Draw(Shader& shader);

};