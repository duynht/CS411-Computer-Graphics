#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <GL/gl3w.h>

#include <glm/glm.hpp>

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
static std::string LoadShader(const std::string& fileName);
static GLuint CreateShader(const std::string& text, GLenum shaderType);


class Shader {
	static const unsigned int NUM_SHADERS = 2;
	
	GLuint program;
	GLuint shaders[NUM_SHADERS];

	std::unordered_map<std::string, GLuint> uniformLocationCache;

public:
	Shader(const std::string& fileName);

	void Bind();
	void Unbind();
	void SetUniformVec4f(const std::string& uniform, const glm::vec4& vec);
	void SetUniformMat4f(const std::string& uniform, const glm::mat4& mat);

	virtual ~Shader();
};