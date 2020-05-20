#pragma once
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <GL\gl3w.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

typedef std::pair<std::string, std::string> ShaderSource;
#define vs first
#define fs second

class Shader {
private:
	GLuint rendererId;
	ShaderSource shaderSource;
	GLboolean compileShader(GLuint type, const std::string& source);
	GLuint createShader();
	ShaderSource parseShader(const std::string& filepath);
	std::unordered_map<std::string, GLuint> uniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	

	void bind() const;
	void unbind() const;

	void setUniformVec4f(const std::string &uniform, const glm::vec4& vec);
	void setUniformMat4f(const std::string &uniform, const glm::mat4& mat);
};