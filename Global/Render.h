#pragma once

#include <vector>

#include "PipelineInterfaces.h"
#include "Render_OGL.h"
#include "Render_Vertex.h"
#include "Render_Shader.h"

#include "nanovg/src/nanovg.h"
#include "Render_BSpline.h"

class Render {
private: //Class members
    BSpline _track;

    //Render state
    int _outputWidthPx;
    int _outputHeightPx;
    float _aspectRatio;
    float _fov;
    float _farDistance;

    Shader _defaultShader;
    Shader _fsqShader;

    //UI state
    NVGcontext* _vg;
    bool _inVGFrame;
    glm::ivec3 _imgAnalysisDebugWindowLoc; //X, Y, Width
    glm::ivec3 _renderDebugWindowLoc; //X, Y, Width

    //OpenGL handles
    unsigned int _carVBO;
    unsigned int _fsqVBO;
    unsigned int _cameraTexture;
public:  //Public methods
    Render();
    ~Render();
    void Init();
    void Step(CameraImageData* camImage, ImageAnalysisResult* imgAnalysis, TrackGeometry* track, float deltaT, GameState* gameState); //Render a frame

    void FramebufferSizeChanged(int width, int height);
    void DrawUIwindow(const char *title, const char *content, float x, float y, float w);
private: //Private methods
    void updateProjectionMatrix();

    void initShaders(); //Load the shader sources and compile them for use
    void initVBOs(); //Load model geometry into Vertex Buffer Objects on the GPU
    void initTextures();

    void uploadCameraImage(const CameraImageData* image);

    void renderUI(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* gameState, const int &trackSegmentCount);
    void renderUIimgAnalysisDebug(const ImageAnalysisResult *imgAnalysis);
    void renderUIrenderingDebug(const int &trackSegmentCount);
    void initUI();
    void drawUIwindowBorder(const char *title, float x, float y, float w, float h);
    void drawUIcontent(const char* content, float x, float y, float w, float h);
};
