#include "Mesh.h"

Vertex::Vertex(const glm::vec3& pos, const glm::vec2& texCoord) {
	this->pos = pos;
	this->texCoord = texCoord;
}

glm::vec3 Vertex::GetPos() const{
	return pos;
}

glm::vec2 Vertex::GetTexCoord() const{
	return texCoord;
}



Mesh::Mesh(Mesh&& other) noexcept {
	vertexArrayObject = std::exchange(other.vertexArrayObject, 0);
	vertexArrayBuffers = std::move(other.vertexArrayBuffers);
	drawCount = std::exchange(other.drawCount, 0);
}

Mesh& Mesh::operator=(Mesh&& other) {
	vertexArrayObject = std::exchange(other.vertexArrayObject, 0);
	vertexArrayBuffers = std::move(other.vertexArrayBuffers);
	drawCount = std::exchange(other.drawCount, 0);

	return *this;
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const unsigned int& numVertices, const std::vector<unsigned int>& indices, const unsigned int& numIndices) {
	vertexArrayBuffers = std::vector<GLuint>(NUM_BUFFERS);

	drawCount = numIndices;

	//m_vertices = std::vector<Vertex>(vertices);
	//m_indices = std::vector<unsigned int>(indices);
	//m_numVertices = numVertices;
	//m_numIndices = numIndices;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	std::vector<glm::vec3> positions;
	positions.reserve(numVertices);
	std::vector<glm::vec2> texCoords;
	texCoords.reserve(numVertices);

	for (unsigned int i = 0; i < numVertices; i++) {
		positions.push_back(vertices[i].GetPos());
		texCoords.push_back(vertices[i].GetTexCoord());
	}

	glCreateBuffers(NUM_BUFFERS, vertexArrayBuffers.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &vertexArrayObject);
}

void Mesh::Draw(GLenum glPrimitive) {
	glBindVertexArray(vertexArrayObject);

//	glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
	glDrawElements(glPrimitive, drawCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

