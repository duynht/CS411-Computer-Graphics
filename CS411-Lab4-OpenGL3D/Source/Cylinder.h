#pragma once
#include "Shape.h"

class Cylinder : public Shape {
	Mesh topMesh;
	Mesh baseMesh;
public:
	Cylinder(float xCenter, float yCenter, float zCenter, float radius, float height, unsigned int sectorCount);
	
	void Draw(Shader& shader);
};