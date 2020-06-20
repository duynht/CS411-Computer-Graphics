#pragma once
#include <vector>
#include <utility>
#include <iostream>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;
public:
	Vertex() = default;
	Vertex(const glm::vec3& pos, const glm::vec2& texCoord = glm::vec2(0.5, 0.5));
	
	glm::vec3 GetPos() const;
	glm::vec2 GetTexCoord() const;

};

class Mesh {
	enum {
		POSITION_VB,
		TEXCOORD_VB,
		INDEX_VB,
		NUM_BUFFERS
	};

	GLuint vertexArrayObject;
	std::vector<GLuint> vertexArrayBuffers;
	unsigned int drawCount;

public:
	Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh& operator =(const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept;
	Mesh& operator =(Mesh&& other);

	Mesh(const std::vector<Vertex>& vertices, const unsigned int& numVertices, const std::vector<unsigned int>& indices, const unsigned int& numIndices);
	
	void Draw(GLenum glPrimitive);

	virtual ~Mesh();
};