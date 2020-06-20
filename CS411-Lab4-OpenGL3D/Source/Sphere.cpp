#include "Sphere.h"

Sphere::Sphere(float xCenter, float yCenter, float zCenter, float radius, unsigned int sectorCount, unsigned int stackCount) {

    this->xCenter = xCenter;
    this->yCenter = yCenter;
    this->zCenter = zCenter;

    std::vector<Vertex> vertices;

    ///////////////////////////////////////////////////////////////////////////////
    // build vertices of sphere with smooth shading using parametric equation
    // x = r * cos(u) * cos(v)
    // y = r * cos(u) * sin(v)
    // z = r * sin(u)
    // where u: stack(latitude) angle (-90 <= u <= 90)
    //       v: sector(longitude) angle (0 <= v <= 360)
    ///////////////////////////////////////////////////////////////////////////////

    float x, y, z, xy;                              // vertex position

    float sectorStep = 2.f * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (unsigned int i = 0; i <= stackCount; i++)
    {
        stackAngle = M_PI / 2.f - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (unsigned int j = 0; j <= sectorCount; j++)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(glm::vec3(x + xCenter, y + yCenter, z + zCenter));
        }
    }

    std::vector<unsigned int> indices;

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (unsigned int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0) {
                indices.push_back(k1); 
                indices.push_back(k2); 
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {   
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    mesh = Mesh(vertices, vertices.size(), indices, indices.size());
}

void Sphere::Draw(Shader& shader) {
    shader.SetUniformMat4f("u_Transform", motion.GetMatrix());
    mesh.Draw(GL_TRIANGLES);
    Clean(shader);
}