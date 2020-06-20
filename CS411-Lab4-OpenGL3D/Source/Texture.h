#pragma once
#include <string>
#include <cassert>
#include <iostream>

#include <GL/gl3w.h>
#include <SOIL/soil.h>

class Texture {
	GLuint texture;

public:
	Texture(const std::string& fileName);

	void Bind(unsigned int unit);
	void Bind();
	void Unbind();

	virtual ~Texture();
};