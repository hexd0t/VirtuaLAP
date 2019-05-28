#pragma once

#include <vector>
#include "glm/glm/glm.hpp"

class Shader {
private:
    glm::mat4 _model;
    glm::mat3 _normal;
    glm::mat4 _view;
    glm::mat4 _proj;

    unsigned int _vertexShader;
    unsigned int _fragmentShader;
    unsigned int _shaderProgram;
    unsigned int _vertexArray;
    unsigned int _diffuseTextureSampler;
    unsigned int _shaderUniformModel;
    unsigned int _shaderUniformNormal;
    unsigned int _shaderUniformView;
    unsigned int _shaderUniformProj;
public:
    Shader();
    ~Shader();
    void Init(const char* sourceName);
    void Apply();
    void SetDiffuseTexture(unsigned int textureHandle);
    void UpdateModel(glm::mat4 transform);
    void UpdateView(glm::mat4 transform);
    void UpdateProj(glm::mat4 transform);
private:
    std::vector<char> readShaderSource(const char* name) const;
    void loadUniforms();
    void uploadModel();
    void uploadView();
    void uploadProj();
};