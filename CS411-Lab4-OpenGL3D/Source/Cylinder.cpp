#include "Cylinder.h"

Cylinder::Cylinder(float xCenter, float yCenter, float zCenter, float radius, float height, unsigned int sectorCount) {
	this->xCenter = xCenter;
	this->yCenter = yCenter;
	this->zCenter = zCenter;

    // get unit circle vectors on XY-plane
    std::vector<float> unitVertices = GetUnitCircleVertices(sectorCount);
    std::vector<Vertex> vertices;
    // put side vertices to arrays
    for (int i = 0; i < 2; ++i) {
        float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2

        for (unsigned int j = 0, k = 0; j <= sectorCount; j++, k += 3) {
            float ux = unitVertices[k];
            float uy = unitVertices[k + 1];
            float uz = unitVertices[k + 2];
            // position vector
            vertices.push_back(Vertex(glm::vec3(ux * radius + xCenter, h + yCenter, uy * radius + zCenter)));
        }
    }

    // the starting index for the base/top surface
    //NOTE: it is used for generating indices later
    int baseCenterIndex = (int)vertices.size();
    int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

    // put base and top vertices to arrays
    for (int i = 0; i < 2; ++i) {
        float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2

        // center point
        vertices.push_back(Vertex(glm::vec3(xCenter, h + yCenter, zCenter)));     

        for (unsigned int j = 0, k = 0; j < sectorCount; j++, k += 3) {
            float ux = unitVertices[k];
            float uy = unitVertices[k + 1];
            // position vector
            vertices.push_back(Vertex(glm::vec3(ux * radius + xCenter, h + yCenter, uy * radius + zCenter )));

        }
    }

    // generate CCW index list of cylinder triangles
    std::vector<unsigned int> indices;
    int k1 = 0;                         // 1st vertex index at base
    int k2 = sectorCount + 1;           // 1st vertex index at top

    // indices for the side surface
    for (unsigned int i = 0; i < sectorCount; i++, k1++, k2++) {
        // 2 triangles per sector
        // k1 => k1+1 => k2
        indices.push_back(k2);
        indices.push_back(k1 + 1);
        indices.push_back(k1);

        // k2 => k1+1 => k2+1
        indices.push_back(k2 + 1);
        indices.push_back(k1 + 1);
        indices.push_back(k2);
    }
    mesh = Mesh(vertices, vertices.size(), indices, indices.size());

    indices.resize(sectorCount + 1);
    // indices for the base surface
    std::iota(indices.begin(), indices.end(), baseCenterIndex);
    *(indices.end() - 1) = baseCenterIndex + 1;
    baseMesh = Mesh(vertices, vertices.size(), indices, indices.size());

    //indices for top surface  
    std::iota(indices.begin(), indices.end(), topCenterIndex);
    *(indices.end() - 1) = topCenterIndex + 1;
    std::reverse(indices.begin() + 1, indices.end());
    topMesh = Mesh(vertices, vertices.size(), indices, indices.size());

}

void Cylinder::Draw(Shader& shader) {
    shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
    topMesh.Draw(GL_TRIANGLE_FAN);
    mesh.Draw(GL_TRIANGLES);
    baseMesh.Draw(GL_TRIANGLE_FAN);
    Clean(shader);
}

