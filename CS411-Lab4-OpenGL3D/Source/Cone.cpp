#include "Cone.h"

Cone::Cone(float xCenter, float yCenter, float zCenter, float radius, float height, unsigned int sectorCount) {
    this->xCenter = xCenter;
    this->yCenter = yCenter;
    this->zCenter = zCenter;

    // get unit circle vectors on XY-plane
    std::vector<float> unitVertices = GetUnitCircleVertices(sectorCount);
    std::vector<Vertex> vertices;

    unsigned int baseCenterIndex = 0;
    unsigned int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

    // center point
    vertices.push_back(Vertex(glm::vec3(xCenter, -height / 4.f + yCenter, zCenter)));
    for (unsigned int i = 0, k = 0; i < sectorCount; i++, k += 3) {
        float ux = unitVertices[k];
        float uy = unitVertices[k + 1];
        // position vector
        vertices.push_back(Vertex(glm::vec3(ux * radius + xCenter, -height / 4.f + yCenter, uy * radius + zCenter)));
    }

    // peak
    vertices.push_back(Vertex(glm::vec3(xCenter, height * 3.f / 4.f + yCenter, zCenter)));

    std::vector<unsigned int> indices(sectorCount + 1);

    // indices for the base surface
    std::iota(indices.begin(), indices.end(), baseCenterIndex);
    *(indices.end() - 1) = baseCenterIndex + 1;
    baseMesh = Mesh(vertices, vertices.size(), indices, indices.size());

    //indices for top surface
    indices[0] = vertices.size() - 1;
    std::reverse(indices.begin() + 1, indices.end());
    mesh = Mesh(vertices, vertices.size(), indices, indices.size());
}

void Cone::Draw(Shader& shader) {
    shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
    mesh.Draw(GL_TRIANGLE_FAN);
    baseMesh.Draw(GL_TRIANGLE_FAN);
    Clean(shader);
}


