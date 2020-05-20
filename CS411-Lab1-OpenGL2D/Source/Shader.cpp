#include "Shader.h"

Shader::Shader(const std::string& filepath) {
   shaderSource = parseShader(filepath);
   rendererId = createShader();
}

Shader::~Shader() {
    glDeleteProgram(rendererId);
}

ShaderSource Shader::parseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                //set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                //set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }

    return std::make_pair(ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::FRAGMENT].str());
}

GLboolean Shader::compileShader(GLuint type, const std::string& source) {
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error handling
    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
            "shader:" << std::endl;
        std::cerr << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

GLuint Shader::createShader() {
    GLuint program = glCreateProgram();
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, shaderSource.vs);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, shaderSource.fs);

    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

void Shader::bind() const {
    glUseProgram(rendererId);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setUniformVec4f(const std::string& uniform, const glm::vec4& vec){
    if (uniformLocationCache.find(uniform) == uniformLocationCache.end()) {
        uniformLocationCache[uniform] = glGetUniformLocation(rendererId, uniform.c_str());
    }
    glUniform4fv(uniformLocationCache[uniform], 1, &vec[0]);
}

void Shader::setUniformMat4f(const std::string& uniform, const glm::mat4& mat){
    if (uniformLocationCache.find(uniform) == uniformLocationCache.end()) {
        uniformLocationCache[uniform] = glGetUniformLocation(rendererId, uniform.c_str());
    }
    glUniformMatrix4fv(uniformLocationCache[uniform], 1, GL_FALSE, &mat[0][0]);
}
