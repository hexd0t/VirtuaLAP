#pragma once

#include <vector>

#include "PipelineInterfaces.h"
#include "Render_OGL.h"
#include "Render_Vertex.h"

class Render {
private: //Class members
    float _aspectRatio;
    float _fov;
    float _farDistance;

    unsigned int _vertexArray;
    unsigned int _carVBO;
    unsigned int _vertexShader;
    unsigned int _fragmentShader;
    unsigned int _shaderProgram;
public:  //Public methods
    Render();
    ~Render();
    void Init();
    void Step(CameraImageData* camImage, ImageAnalysisResult* imgAnalysis, TrackGeometry* track, float deltaT, GameState* gameState); //Render a frame

    void FramebufferSizeChanged(int width, int height);
private: //Private methods
    void updateProjectionMatrix();
    void initShaders(); //Load the shader sources and compile them for use
    void initVBOs(); //Load model geometry into Vertex Buffer Objects on the GPU
    unsigned int createVertexBuffer(const std::vector<Vertex>& vertices) const;
    std::vector<char> readShaderSource(const char* name) const;
};
