#pragma once
#include "Shape.h"

class Cube : public Shape{

public:
	Cube(float xCenter, float yCenter, float zCenter, float edgeLength);
	void Draw(Shader& shader);
};

