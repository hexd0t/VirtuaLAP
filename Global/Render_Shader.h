#pragma once

#include <vector>

class Shader {
private:
    unsigned int _vertexShader;
    unsigned int _fragmentShader;
    unsigned int _shaderProgram;
    unsigned int _diffuseTextureSampler;
public:
    Shader();
    ~Shader();
    void Init(const char* sourceName);
    void Apply();
    void SetDiffuseTexture(unsigned int textureHandle);
private:
    std::vector<char> readShaderSource(const char* name) const;
    void loadUniforms();
};