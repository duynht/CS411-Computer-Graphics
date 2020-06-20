#include "Circle.h"

Circle::Circle(float xCenter, float yCenter, float zCenter, float radius, unsigned int sectorCount) {
	this->radius = radius;
	this->xCenter = xCenter;
	this->yCenter = yCenter;
	this->zCenter = zCenter;

	std::vector<float> unitVertices = GetUnitCircleVertices(sectorCount);
	std::vector<Vertex> vertices;

	for (unsigned int i = 0, k = 0; i <= sectorCount; i++, k += 3) {
		float ux = unitVertices[k];
		float uy = unitVertices[k + 1];
		float uz = unitVertices[k + 2];
		// position vector
		vertices.push_back(Vertex(glm::vec3(ux * radius + xCenter, yCenter, uy * radius + zCenter)));
	}

	std::vector<unsigned int> indices(vertices.size() + 1);
	std::iota(indices.begin(), indices.end(), 0);
	*(indices.end() - 1) = 1;
	std::reverse(indices.begin() + 1, indices.end()); //drawing face-up

	mesh = Mesh(vertices, vertices.size(), indices, vertices.size());
}

void Circle::Draw(Shader& shader) {
	shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
	mesh.Draw(GL_TRIANGLE_FAN);
	Clean(shader);
}
