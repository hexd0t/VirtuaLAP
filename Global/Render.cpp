#include "Render.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <zconf.h>

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"
#include "Render_Constants.h"

Render::Render() :
    _outputWidthPx(1280), _outputHeightPx(768),
    _aspectRatio(1.333f), _fov(45.f), _farDistance(5000),

    _vg(nullptr), _imgAnalysisDebugWindowLoc(20, 20, 200)
{

}

Render::~Render() {

}

void DebugPrint(const char* c) {
    std::cerr << c << std::endl;
}

void Render::Step(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* gameState) {
    glClearColor(0.8f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindVertexArray(_vertexArray);

    uploadCameraImage(camImage);

    _fsqShader.Apply();
    glBindBuffer(GL_ARRAY_BUFFER, _fsqVBO);
    _fsqShader.SetDiffuseTexture(_cameraTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    _defaultShader.Apply();
    glBindBuffer(GL_ARRAY_BUFFER, _carVBO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //renderUI(camImage, imgAnalysis, track, deltaT, gameState);
}

void Render::FramebufferSizeChanged(int width, int height) {
    _outputWidthPx = width;
    _outputHeightPx = height;
    glViewport( 0, 0, width, height );
    _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    updateProjectionMatrix();
}

void Render::updateProjectionMatrix() {
    glm::mat4 proj = glm::perspective( _fov, _aspectRatio, 0.1f, _farDistance );
}

void Render::initShaders() {
    _defaultShader.Init("default");
    _fsqShader.Init("fsq");
}

void Render::initVBOs() {
    glGenVertexArrays(1, &_vertexArray); //Note(AMü): if we use multiple shaders, each might need their own
    glBindVertexArray(_vertexArray);
    Vertex::SetLayout();

    std::vector<Vertex> fsqVertices;
    fsqVertices.emplace_back(-1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    fsqVertices.emplace_back(-1.f,  1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f);
    fsqVertices.emplace_back( 1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    fsqVertices.emplace_back( 1.f,  1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
    std::vector<Vertex> carVertices;
    carVertices.emplace_back(-0.8f, -0.8f,  0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    carVertices.emplace_back(  0.f,  0.8f,  0.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    carVertices.emplace_back( 0.8f, -0.8f, 0.2f, 0.f, 0.f, 1.f, 0.f, 1.f);

    glBindVertexArray(_vertexArray);
    _carVBO = createVertexBuffer(carVertices);
    _fsqVBO = createVertexBuffer(fsqVertices);
}

unsigned int Render::createVertexBuffer(const std::vector<Vertex>& vertices) const {
    GLuint vertexBuffer;
    glGenBuffers( 1, &vertexBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), &vertices[0], GL_STATIC_DRAW );
    return vertexBuffer;
}

void Render::Init() {

    initShaders();
    initVBOs();
    initTextures();
    initUI();
}

void Render::renderUI(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* gameState) {
    nvgBeginFrame(_vg, _outputWidthPx, _outputHeightPx, 1.0f);

    //renderUIimgAnalysisDebug(imgAnalysis);
    nvgBeginPath(_vg);
    nvgRect(_vg, 100,100, 120,30);
    nvgFillColor(_vg, nvgRGBA(255,192,0,255));
    nvgFill(_vg);

    nvgEndFrame(_vg);
}

void Render::drawUIcontent(const char *content, float x, float y, float w, float h) {
    nvgSave(_vg);

    nvgFontSize(_vg, 16.0f);
    nvgFontFace(_vg, "arial");
    nvgTextAlign(_vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);

    nvgFontBlur(_vg,0);
    nvgFillColor(_vg, nvgRGBA(200,200,200,180));
    nvgTextBox(_vg, x + UI_WINDOW_CONTENTPADDING, y + UI_WINDOW_CONTENTPADDING_TOP,
            w - 2*UI_WINDOW_CONTENTPADDING, content, nullptr);

    nvgRestore(_vg);
}

void Render::drawUIwindowBorder(const char *title, float x, float y, float w, float h) {
    const float cornerRadius = 5.0f;
    NVGpaint shadowPaint;
    NVGpaint headerPaint;

    nvgSave(_vg);

    // Window
    nvgBeginPath(_vg);
    nvgRoundedRect(_vg, x,y, w,h, cornerRadius);
    nvgFillColor(_vg, nvgRGBA(28,30,34,192));
    nvgFill(_vg);

    // Drop shadow
    shadowPaint = nvgBoxGradient(_vg, x,y+2, w,h, cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
    nvgBeginPath(_vg);
    nvgRect(_vg, x-10,y-10, w+20,h+30);
    nvgRoundedRect(_vg, x,y, w,h, cornerRadius);
    nvgPathWinding(_vg, NVG_HOLE);
    nvgFillPaint(_vg, shadowPaint);
    nvgFill(_vg);

    // Header
    headerPaint = nvgLinearGradient(_vg, x,y,x,y+15, nvgRGBA(255,255,255,8), nvgRGBA(0,0,0,16));
    nvgBeginPath(_vg);
    nvgRoundedRect(_vg, x+1,y+1, w-2,30, cornerRadius-1);
    nvgFillPaint(_vg, headerPaint);
    nvgFill(_vg);
    nvgBeginPath(_vg);
    nvgMoveTo(_vg, x+0.5f, y+0.5f+30);
    nvgLineTo(_vg, x+0.5f+w-1, y+0.5f+30);
    nvgStrokeColor(_vg, nvgRGBA(0,0,0,32));
    nvgStroke(_vg);

    nvgFontSize(_vg, 18.0f);
    nvgFontFace(_vg, "arial");
    nvgTextAlign(_vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);

    nvgFontBlur(_vg,2);
    nvgFillColor(_vg, nvgRGBA(0,0,0,128));
    nvgText(_vg, x+w/2,y+16+1, title, nullptr);

    nvgFontBlur(_vg,0);
    nvgFillColor(_vg, nvgRGBA(220,220,220,160));
    nvgText(_vg, x+w/2,y+16, title, nullptr);

    nvgRestore(_vg);
}

void Render::initUI() {
    _vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

    //Note(AMü): Since these will never be unloaded, we don't store the handles returned
#if defined _WIN32
    nvgCreateFont(_vg, "arial", "C:/Windows/Fonts/arial.ttf");
#else
#error ToDo: implement non-Windows font loading
#endif
}

void Render::drawUIwindow(const char *title, const char *content, float x, float y, float w) {
    float contentbounds[4] = {0.f};
    nvgTextBoxBounds(_vg, x, y, w - 2 * UI_WINDOW_CONTENTPADDING, content, nullptr, contentbounds);
    float windowheight = contentbounds[3] - contentbounds[1] + UI_WINDOW_CONTENTPADDING_TOP + UI_WINDOW_CONTENTPADDING;
    //std::cout << title << " " << x << " " << y << " " << w << " " << windowheight << std::endl;
    drawUIwindowBorder(title, x, y, w, windowheight);
    drawUIcontent(content, x, y, w, windowheight);
}

void Render::renderUIimgAnalysisDebug(const ImageAnalysisResult *imgAnalysis) {
    std::stringstream content;
    content.setf(std::ios::fixed, std::ios::floatfield);
    content.setf(std::ios::showpoint); //Always show float decimal place
    content << "State: ";
    switch (imgAnalysis->State) {
        default:
        case ImageAnalysis_Unknown:
            content << "Unknown";
            break;
        case ImageAnalysis_Calibrating:
            content << "Calibrating";
            break;
        case ImageAnalysis_Operating:
            content << "Operating";
            break;
    }
    content << std::endl << std::setprecision(1) <<
            "Camera Location:" << std::endl <<
            "X: " << std::setw(8) << imgAnalysis->CameraLocation.x << std::endl <<
            "Y: " << std::setw(8) << imgAnalysis->CameraLocation.y << std::endl <<
            "Z: " << std::setw(8) << imgAnalysis->CameraLocation.z << std::endl;
    content << "Currently tracking markers:" << std::endl << std::setfill('0');
    for(auto & MarkerLocation : imgAnalysis->MarkerLocations) {
        content << std::setw(0) << MarkerLocation.first << std::endl;
    }
    auto loc = _imgAnalysisDebugWindowLoc;
    drawUIwindow("ImageAnalysis Data", content.str().c_str(), loc.x, loc.y, loc.z);
}

void Render::initTextures() {
    glGenTextures(1, &_cameraTexture);
}

void Render::uploadCameraImage(const CameraImageData *image) {
    glBindTexture(GL_TEXTURE_2D, _cameraTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->Width, image->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->Data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete[] image->Data; //ToDo(AMü): Move allocation to Global/Core and reuse buffer
}

