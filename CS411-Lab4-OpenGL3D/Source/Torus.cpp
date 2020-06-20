#include "Torus.h"

Torus::Torus(float xCenter, float yCenter, float zCenter, float mainRadius, float tubeRadius, unsigned int mainSectorCount, unsigned int tubeSectorCount) {
	this->xCenter = xCenter;
	this->yCenter = yCenter;
	this->zCenter = zCenter;

	std::vector<Vertex> vertices;
	vertices.reserve((mainSectorCount + 1)* (tubeSectorCount + 1));
	std::vector<unsigned int> indices;
	indices.reserve((mainSectorCount * 2 * (tubeSectorCount + 1)) + mainSectorCount - 1);

	float mainSectorAngleStep = 2.f * M_PI / mainSectorCount;
	float tubeSectorAngleStep = 2.f * M_PI / tubeSectorCount;

	float mainSectorAngle = 0.f;
	for (unsigned int i = 0; i <= mainSectorCount; i++) {
		float mainSin = sin(mainSectorAngle);
		float mainCos = cos(mainSectorAngle);

		float tubeSectorAngle = 0.f;
		for (unsigned int j = 0; j <= tubeSectorCount; j++) {
			float tubeSin = sin(tubeSectorAngle);
			float tubeCos = cos(tubeSectorAngle);

			vertices.push_back(Vertex(glm::vec3(xCenter + (mainRadius + tubeRadius * tubeCos) * mainCos,
													yCenter + tubeRadius * tubeSin,
													zCenter + (mainRadius + tubeRadius * tubeCos) * mainSin)));

			tubeSectorAngle += tubeSectorAngleStep;
		}
		mainSectorAngle += mainSectorAngleStep;
	}

	primitiveRestartIndex = vertices.size();

	for (unsigned int i = 0; i < mainSectorCount; i++) {
		for (unsigned int j = 0; j <= tubeSectorCount; j++) {
			indices.push_back(i * (tubeSectorCount + 1) + j + tubeSectorCount + 1);
			indices.push_back(i * (tubeSectorCount + 1) + j);			
		}
		if (i != mainSectorCount - 1) {
			indices.push_back(primitiveRestartIndex);
		}
	}

	//std::reverse(indices.begin(), indices.end());

	mesh = Mesh(vertices, vertices.size(), indices, indices.size());
	
}

void Torus::Draw(Shader& shader) {
    shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(primitiveRestartIndex);
    mesh.Draw(GL_TRIANGLE_STRIP);
	glDisable(GL_PRIMITIVE_RESTART);
    Clean(shader);
}
