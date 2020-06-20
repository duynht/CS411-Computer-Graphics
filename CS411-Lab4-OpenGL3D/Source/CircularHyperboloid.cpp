#include "CircularHyperboloid.h"

CircularHyperboloid::CircularHyperboloid(float xCenter, float yCenter, float zCenter, float hipRadius, float hyperbolaFocus, unsigned int sectorCount, unsigned int stackCount) {
	this->xCenter = xCenter;
	this->yCenter = yCenter;
	this->zCenter = zCenter;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float revolvingStep = 2.f * M_PI / sectorCount;
	float elevationStep = 2.f * M_PI / stackCount;

	float revolvingAngle = 0.f;
	float elevationAngle = -M_PI;
	for (unsigned int i = 0; i <= stackCount + 4; i++) {
		revolvingAngle = 0.f;
		for (unsigned int j = 0; j < sectorCount ; j++) {
			float uSin = sin(revolvingAngle);
			float uCos = cos(revolvingAngle);

			vertices.push_back(Vertex(glm::vec3(xCenter + hipRadius * (uCos - elevationAngle * uSin),
				yCenter + hyperbolaFocus * elevationAngle,
				zCenter + hipRadius * (uSin + elevationAngle * uCos))));

			revolvingAngle += revolvingStep;
		}
		elevationAngle += elevationStep;
		
	}


	for (unsigned int i = 0; i < stackCount; i++) {
		for (unsigned int j = 0; j < sectorCount; j++) {
			indices.push_back(i * (sectorCount + 1) + j + 1);
			indices.push_back(i * (sectorCount + 1) + j );
			indices.push_back(i * (sectorCount + 1) + j + sectorCount + 1);
			

			
			indices.push_back(i * (sectorCount + 1) + j + sectorCount + 2);
			indices.push_back(i * (sectorCount + 1) + j + 1);
			indices.push_back(i * (sectorCount + 1) + j + sectorCount + 1);
		}
	}

	mesh = Mesh(vertices, vertices.size(), indices, indices.size());
	
	vertices.clear();

	revolvingAngle = 0.f;
	elevationAngle = -M_PI;
	vertices.push_back(Vertex(glm::vec3(xCenter, yCenter - hyperbolaFocus * elevationAngle, zCenter)));
	for (unsigned int i = 0, k = 0; i <= sectorCount; i++, k += 3) {	
		float uSin = sin(revolvingAngle);
		float uCos = cos(revolvingAngle);
		vertices.push_back(Vertex(glm::vec3(xCenter + hipRadius * (uCos - elevationAngle * uSin),
			yCenter - hyperbolaFocus * elevationAngle,
			zCenter + hipRadius * (uSin + elevationAngle * uCos))));
		revolvingAngle += revolvingStep;
	}

	indices.resize(vertices.size() + 1);
	std::iota(indices.begin(), indices.end(), 0);
	*(indices.end() - 1) = 1;

	baseMesh = Mesh(vertices, vertices.size(), indices, indices.size());

	vertices.clear();
	revolvingAngle = 0.f;
	elevationAngle = M_PI;
	vertices.push_back(Vertex(glm::vec3(xCenter, yCenter + hyperbolaFocus * elevationAngle, zCenter)));
	for (unsigned int i = 0, k = 0; i <= sectorCount; i++, k += 3) {
		float uSin = sin(revolvingAngle);
		float uCos = cos(revolvingAngle);
		vertices.push_back(Vertex(glm::vec3(xCenter + hipRadius * (uCos - elevationAngle * uSin),
			yCenter + hyperbolaFocus * elevationAngle,
			zCenter + hipRadius * (uSin + elevationAngle * uCos))));
		revolvingAngle += revolvingStep;
	}
	std::reverse(indices.begin() + 1, indices.end());

	topMesh = Mesh(vertices, vertices.size(), indices, indices.size());
}

void CircularHyperboloid::Draw(Shader& shader) {
	shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
	
	topMesh.Draw(GL_TRIANGLE_FAN);
	mesh.Draw(GL_TRIANGLES);
	baseMesh.Draw(GL_TRIANGLE_FAN);
	Clean(shader);
}
