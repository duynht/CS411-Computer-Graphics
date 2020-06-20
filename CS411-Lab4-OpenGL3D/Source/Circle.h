#pragma once
#include "Shape.h"

class Circle : public Shape {
	float radius;
public:
	Circle(float xCenter, float yCenter, float zCenter, float radius, unsigned int sectorCount);
	void Draw(Shader& shader);
};