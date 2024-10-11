#pragma once
#include "common.hpp"

class Shader {
public:
    Shader(const std::string& folderPath);
    void use();
    void setMat4(const std::string& name, const float* value);
    ~Shader();

private:
    GLuint m_shaderProgram;
    void checkCompileErrors(unsigned int shader, const std::string& type);
    std::string readShaderFile(const std::string& filePath);
};