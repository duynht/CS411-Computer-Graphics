#include "CircularParaboloid.h"

CircularParaboloid::CircularParaboloid(float xCenter, float yCenter, float zCenter, float skirtRadius, float height, unsigned int sectorCount, unsigned int stackCount) {
	this->xCenter = xCenter;
	this->yCenter = yCenter;
	this->zCenter = zCenter;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float revolvingStep = 2.f * M_PI / sectorCount;
	float revolvingAngle = 0.f;
	float heightStep = height / stackCount;
	float h = 0.f;
	for (unsigned int i = 0; i <= stackCount + 1; i++) {
		revolvingAngle = 0.f;

		for (unsigned int j = 0; j < sectorCount; j++) {
			float uSin = sin(revolvingAngle);
			float uCos = cos(revolvingAngle);

			vertices.push_back(Vertex(glm::vec3(xCenter + skirtRadius * sqrtf(h) * uSin,
												yCenter - h,
												zCenter + skirtRadius * sqrtf(h) * uCos)));

			revolvingAngle += revolvingStep;
		}
		h += heightStep;
	}


	for (unsigned int i = 0; i < stackCount; i++) {
		for (unsigned int j = 0; j < sectorCount - 1; j++) {
			indices.push_back(i * (sectorCount + 1) + j + 1);
			indices.push_back(i * (sectorCount + 1) + j);
			indices.push_back(i * (sectorCount + 1) + j + sectorCount + 1);



			indices.push_back(i * (sectorCount + 1) + j + sectorCount + 2);
			indices.push_back(i * (sectorCount + 1) + j + 1);
			indices.push_back(i * (sectorCount + 1) + j + sectorCount + 1);
		}
	}

	mesh = Mesh(vertices, vertices.size(), indices, indices.size());

	vertices.clear();

	revolvingAngle = 0.f;
	vertices.push_back(Vertex(glm::vec3(xCenter, yCenter - height, zCenter)));
	for (unsigned int i = 0, k = 0; i <= sectorCount; i++, k += 3) {
		float uSin = sin(revolvingAngle);
		float uCos = cos(revolvingAngle);
		vertices.push_back(Vertex(glm::vec3(xCenter + skirtRadius * uCos,
			yCenter - height,
			zCenter + skirtRadius * uSin)));
		revolvingAngle += revolvingStep;
	}

	indices.resize(vertices.size() + 1);
	std::iota(indices.begin(), indices.end(), 0);
	*(indices.end() - 1) = 1;

	baseMesh = Mesh(vertices, vertices.size(), indices, indices.size());
}

void CircularParaboloid::Draw(Shader& shader) {
	shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
	mesh.Draw(GL_TRIANGLES);
	baseMesh.Draw(GL_TRIANGLE_FAN);
	Clean(shader);
}
