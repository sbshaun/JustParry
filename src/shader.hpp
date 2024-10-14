#pragma once
#include "common.hpp"

class Shader {
public:
    Shader(const std::string& folderPath);
    void use();
    void setMat4(const std::string& name, const glm::mat4& trans);
    void setInt(const std::string& name, int value);
    void setBool(const std::string& name, bool value);
    ~Shader();
    GLuint m_shaderProgram;

private:
    void checkCompileErrors(unsigned int shader, const std::string& type);
    std::string readShaderFile(const std::string& filePath);
};