#include "Shader.h"

static std::string LoadShader(const std::string& fileName) {
	std::ifstream file;
	file.open(fileName.c_str());

	std::string output;
	std::string line;

	if (file.is_open()) {
		while (file.good()) {
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
}

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage) {
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram) {
		glGetProgramiv(shader, flag, &success);
	}
	else {
		glGetShaderiv(shader, flag, &success);
	}

	if (success == GL_FALSE) {
		if (isProgram) {
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		}
		else {
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		}

		std::cerr << errorMessage << ": " << error << std::endl;
	}
}

 static GLuint CreateShader(const std::string& text, GLenum shaderType) {
	 GLuint shader = glCreateShader(shaderType);

	 if (shader == 0) {
		 std::cerr << "Error: Shader creation failed!" << std::endl;
	 }

	 const GLchar* shaderSourceStrings[1];
	 GLint shaderSourceStringLengths[1];

	 shaderSourceStrings[0] = text.c_str();
	 shaderSourceStringLengths[0] = text.length();

	 glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);

	 glCompileShader(shader);

	 CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed");

	 return shader;
}

Shader::Shader(const std::string& fileName) {
	program = glCreateProgram();
	shaders[0] = CreateShader(LoadShader(fileName + "Vertex.glsl"), GL_VERTEX_SHADER);
	shaders[1] = CreateShader(LoadShader(fileName + "Fragment.glsl"), GL_FRAGMENT_SHADER);

	for (auto i = 0; i < NUM_SHADERS; i++) {
		glAttachShader(program, shaders[i]);
	}

	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: Program linking failed");

	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: Program is invalid");
}

Shader::~Shader() {

	for (auto i = 0; i < NUM_SHADERS; i++) {
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	glDeleteProgram(program);
}

void Shader::Bind() {
	glUseProgram(program);
}

void Shader::Unbind() {
	glUseProgram(0);
}

void Shader::SetUniformVec4f(const std::string& uniform, const glm::vec4& vec) {
	if (uniformLocationCache.find(uniform) == uniformLocationCache.end()) {
		uniformLocationCache[uniform] = glGetUniformLocation(program, uniform.c_str());
	}
	glUniform4fv(uniformLocationCache[uniform], 1, &vec[0]);
}

void Shader::SetUniformMat4f(const std::string& uniform, const glm::mat4& mat) {
	if (uniformLocationCache.find(uniform) == uniformLocationCache.end()) {
		uniformLocationCache[uniform] = glGetUniformLocation(program, uniform.c_str());
	}
	glUniformMatrix4fv(uniformLocationCache[uniform], 1, GL_FALSE, &mat[0][0]);
}

