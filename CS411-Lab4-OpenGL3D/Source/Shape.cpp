#include "Shape.h"

std::vector<float> Shape::GetUnitCircleVertices(unsigned int sectorCount) {
    float sectorStep = 2.f * M_PI / sectorCount;
    float sectorAngle;  // radian

    std::vector<float> unitCircleVertices;
    for (unsigned int i = 0; i <= sectorCount; ++i) {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back(cos(sectorAngle)); // x
        unitCircleVertices.push_back(sin(sectorAngle)); // y
        unitCircleVertices.push_back(0);                // z
    }
    return unitCircleVertices;
}


void Shape::Clean(Shader& shader) {
	shader.SetUniformMat4f("u_Transform", glm::mat4(1));
}

void Shape::Rotate(glm::vec3 radians, glm::vec3 axis) {
	if (axis == glm::vec3()) {
		glm::vec3 self_axis(xCenter, yCenter, zCenter);
		motion.SetTranslate(self_axis);
		self_axis *= -1;
		motion.SetNegTranslate(self_axis);
	}

	motion.SetRotate(radians);
}

