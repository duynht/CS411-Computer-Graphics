#include "Cube.h"

Cube::Cube(float xCenter, float yCenter, float zCenter, float edgeLength) {
	this->xCenter = xCenter;
	this->yCenter = yCenter;
	this->zCenter = zCenter;

	float halfLength = edgeLength * 0.5;

	unsigned int numVertices = 8;
	unsigned int numIndices = 6 * 2 * 3; //GL_QUADS is deprecated, using GL_TRIANGLES

	std::vector<Vertex> vertices = {
		glm::vec3(xCenter - halfLength, yCenter - halfLength, zCenter + halfLength), // front bottom left
		glm::vec3(xCenter + halfLength, yCenter - halfLength, zCenter + halfLength), // front bottom right
		glm::vec3(xCenter + halfLength, yCenter + halfLength, zCenter + halfLength), // front top right
		glm::vec3(xCenter - halfLength, yCenter + halfLength, zCenter + halfLength), // front top left
		glm::vec3(xCenter - halfLength, yCenter - halfLength, zCenter - halfLength), // back bottom left
		glm::vec3(xCenter + halfLength, yCenter - halfLength, zCenter - halfLength), // back bottom right
		glm::vec3(xCenter + halfLength, yCenter + halfLength, zCenter - halfLength), // back top right
		glm::vec3(xCenter - halfLength, yCenter + halfLength, zCenter - halfLength), // back top left		
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0,		

		3, 2, 6,
		6, 7, 3,

		6, 5, 4,
		4, 7, 6,

		4, 5, 1,
		1, 0, 4,

		0, 3, 7,
		7, 4, 0,

		1, 5, 6,
		6, 2, 1,
	};

	mesh = Mesh(vertices, numVertices, indices, numIndices);
}

void Cube::Draw(Shader& shader) {
	shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
	mesh.Draw(GL_TRIANGLES);
	Clean(shader);
}
