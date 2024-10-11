#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string Shader::readShaderFile(const std::string& filePath) {
    std::cout << "Loading shader filename: " << filePath << std::endl;

    std::ifstream ifs(filePath);

    if (!ifs.good())
    {
        std::cerr << "ERROR: invalid filename loading shader from file: " << filePath << std::endl;
        return "";
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    std::cout << oss.str() << std::endl;
    return oss.str();
}

Shader::Shader(const std::string& folderPath) {
    std::string vertexCode = readShaderFile(PROJECT_SOURCE_DIR + std::string("/shaders/") + folderPath + std::string("/vs.glsl"));
	std::string fragmentCode = readShaderFile(PROJECT_SOURCE_DIR + std::string("/shaders/") + folderPath + std::string("/fs.glsl"));
    gl_has_errors();
    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();
    gl_has_errors();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");
	
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    gl_has_errors();
    // Shader Program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    checkCompileErrors(m_shaderProgram, "PROGRAM");
    gl_has_errors();
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    gl_has_errors();
}

void Shader::use() {
    glUseProgram(m_shaderProgram);
}

void Shader::setMat4(const std::string& name, const float* value) {
    GLuint location = glGetUniformLocation(m_shaderProgram, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

Shader::~Shader() {
    glDeleteProgram(m_shaderProgram);
    std::cout << "Shader program deleted." << std::endl;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
}
