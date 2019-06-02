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
    _aspectRatio(1.333f), _fov(3.141f*0.5f), _farDistance(20000),

    _vg(nullptr), _inVGFrame(false), _imgAnalysisDebugWindowLoc(20, 20, 200),
    _renderDebugWindowLoc(460, 20, 200)
{

}

Render::~Render() {

}

void DebugPrint(const char* c) {
    std::cerr << c << std::endl;
}

void Render::Step(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* gameState) {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uploadCameraImage(camImage);

    glDisable(GL_DEPTH_TEST);
    _fsqShader.Apply();
    glBindBuffer(GL_ARRAY_BUFFER, _fsqVBO);
    _fsqShader.SetDiffuseTexture(_cameraTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    _defaultShader.Apply();
    _defaultShader.UpdateView(glm::lookAt(
            imgAnalysis->CameraLocation,
            imgAnalysis->CameraLocation + imgAnalysis->CameraLookDirection,
            imgAnalysis->CameraUp
            ));

    _defaultShader.UpdateModel(glm::mat4(1.0f));

    if(imgAnalysis->State != ImageAnalysis_MarkerOutput) {
        std::vector<Vertex> trackVertices;
        auto points = _track.Extrude(_track.Discretize(track));
        for (auto &point : points) {
            trackVertices.emplace_back(point, glm::vec3(0, 0, 1), glm::vec2(0.f, 0.f));
        }
        unsigned int trackVB = CreateVertexBuffer(trackVertices);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, trackVertices.size());
        glDeleteBuffers(1, &trackVB);


        renderUI(camImage, imgAnalysis, track, deltaT, gameState, points.size());
    }
}

void Render::FramebufferSizeChanged(int width, int height) {
    _outputWidthPx = width;
    _outputHeightPx = height;
    glViewport( 0, 0, width, height );
    _aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    updateProjectionMatrix();
}

void Render::updateProjectionMatrix() {
    _defaultShader.UpdateProj(glm::perspective( _fov, _aspectRatio, 0.1f, _farDistance ));
}

void Render::initShaders() {
    _defaultShader.Init("default");
    _fsqShader.Init("fsq");
    updateProjectionMatrix();
}

void Render::initVBOs() {

    std::vector<UVVertex> fsqVertices;
    fsqVertices.emplace_back(-1.f, -1.f, 0.f, 0.f, 1.f);
    fsqVertices.emplace_back(-1.f,  1.f, 0.f, 0.f, 0.f);
    fsqVertices.emplace_back( 1.f, -1.f, 0.f, 1.f, 1.f);
    fsqVertices.emplace_back( 1.f,  1.f, 0.f, 1.f, 0.f);
    std::vector<Vertex> carVertices;
    //Top:
    carVertices.emplace_back( 20.f, -20.f,  20.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    carVertices.emplace_back( 20.f,  20.f,  20.f, 0.f, 0.f, 1.f, 1.f, 1.f);
    carVertices.emplace_back(-20.f, -20.f,  20.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    carVertices.emplace_back(-20.f,  20.f,  20.f, 0.f, 0.f, 1.f, 0.f, 1.f);
    //Front:
    carVertices.emplace_back( 20.f, -20.f,  20.f, 1.f, 0.f, 0.f, 1.f, 0.f);
    carVertices.emplace_back( 20.f, -20.f, -20.f, 1.f, 0.f, 0.f, 0.f, 0.f);
    carVertices.emplace_back( 20.f,  20.f,  20.f, 1.f, 0.f, 0.f, 1.f, 1.f);
    carVertices.emplace_back( 20.f,  20.f, -20.f, 1.f, 0.f, 0.f, 0.f, 1.f);
    //Right:
    carVertices.emplace_back( 20.f,  20.f,  20.f, 0.f, 1.f, 0.f, 1.f, 0.f);
    carVertices.emplace_back( 20.f,  20.f, -20.f, 0.f, 1.f, 0.f, 0.f, 0.f);
    carVertices.emplace_back(-20.f,  20.f,  20.f, 0.f, 1.f, 0.f, 1.f, 1.f);
    carVertices.emplace_back(-20.f,  20.f, -20.f, 0.f, 1.f, 0.f, 0.f, 1.f);
    //Back:
    carVertices.emplace_back(-20.f, -20.f,  20.f, -1.f, 0.f, 0.f, 1.f, 0.f);
    carVertices.emplace_back(-20.f,  20.f,  20.f, -1.f, 0.f, 0.f, 1.f, 1.f);
    carVertices.emplace_back(-20.f, -20.f, -20.f, -1.f, 0.f, 0.f, 0.f, 0.f);
    carVertices.emplace_back(-20.f,  20.f, -20.f, -1.f, 0.f, 0.f, 0.f, 1.f);
    //Bottom
    carVertices.emplace_back( 20.f, -20.f, -20.f, 0.f, 0.f, -1.f, 1.f, 0.f);
    carVertices.emplace_back(-20.f, -20.f, -20.f, 0.f, 0.f, -1.f, 0.f, 0.f);
    carVertices.emplace_back( 20.f,  20.f, -20.f, 0.f, 0.f, -1.f, 1.f, 1.f);
    carVertices.emplace_back(-20.f,  20.f, -20.f, 0.f, 0.f, -1.f, 0.f, 1.f);
    //Left:
    carVertices.emplace_back( 20.f, -20.f,  20.f, 0.f, -1.f, 0.f, 1.f, 0.f);
    carVertices.emplace_back(-20.f, -20.f,  20.f, 0.f, -1.f, 0.f, 1.f, 1.f);
    carVertices.emplace_back( 20.f, -20.f, -20.f, 0.f, -1.f, 0.f, 0.f, 0.f);
    carVertices.emplace_back(-20.f, -20.f, -20.f, 0.f, -1.f, 0.f, 0.f, 1.f);


    _defaultShader.Apply();
    _carVBO = CreateVertexBuffer(carVertices);
    _fsqShader.Apply();
    _fsqVBO = CreateVertexBuffer(fsqVertices);
}

void Render::Init() {
    glClearStencil(0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    initShaders();
    initVBOs();
    initTextures();
    initUI();
}

void Render::renderUI(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track,
        float deltaT, GameState* gameState, const int &trackSegmentCount) {
    nvgBeginFrame(_vg, _outputWidthPx, _outputHeightPx, 1.0f);
    _inVGFrame = true;

    renderUIimgAnalysisDebug(imgAnalysis);
    renderUIrenderingDebug(trackSegmentCount);

    _inVGFrame = false;
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
    nvgCreateFont(_vg, "arial", "/usr/share/fonts/TTF/DejaVuSans.ttf");
#endif
}

void Render::DrawUIwindow(const char *title, const char *content, float x, float y, float w) {
    if(!_inVGFrame) //If the caller isn't providing a VG Frame, start one juAn overview is supposed to give an informational overview of the available options, their respective advantages and disadvantages. If it doesn't do that at all, I don't see how it could be a good overview.st for this element
        nvgBeginFrame(_vg, _outputWidthPx, _outputHeightPx, 1.0f);

    float contentbounds[4] = {0.f};
    nvgTextBoxBounds(_vg, x, y, w - 2 * UI_WINDOW_CONTENTPADDING, content, nullptr, contentbounds);
    float windowheight = contentbounds[3] - contentbounds[1] + UI_WINDOW_CONTENTPADDING_TOP + UI_WINDOW_CONTENTPADDING;
    //std::cout << title << " " << x << " " << y << " " << w << " " << windowheight << std::endl;
    drawUIwindowBorder(title, x, y, w, windowheight);
    drawUIcontent(content, x, y, w, windowheight);

    if(!_inVGFrame)
        nvgEndFrame(_vg);
}

void Render::renderUIimgAnalysisDebug(const ImageAnalysisResult *imgAnalysis) {
    std::stringstream content;
    content.setf(std::ios::fixed, std::ios::floatfield);
    content.setf(std::ios::showpoint); //Always show float decimal place
    content << "State: ";
    switch (imgAnalysis->State & 0xFF) {
        default:
            content << std::hex << imgAnalysis->State << "!";
        case ImageAnalysis_Unknown:
            content << "Unknown";
            break;
        case ImageAnalysis_Calibrating:
            content << "Calibrating";
            break;
        case ImageAnalysis_Operating:
            content << "Operating";
            break;
        case ImageAnalysis_Simulating:
            content << "Simulating";
            break;
    }
    if(imgAnalysis->State & ImageAnalysis_DebugOverlay)
        content << " (Debug)";
    content << std::endl;
    content << "Calib err: " << imgAnalysis->CalibrationError << std::endl;
    content << std::setprecision(1) <<
            "Camera Location:" << std::endl <<
            "X: " << std::setw(8) << imgAnalysis->CameraLocation.x << std::endl <<
            "Y: " << std::setw(8) << imgAnalysis->CameraLocation.y << std::endl <<
            "Z: " << std::setw(8) << imgAnalysis->CameraLocation.z << std::endl;
    content << "Currently known markers:" << std::endl << std::setfill('0');
    for(auto & MarkerLocation : imgAnalysis->Markers) {
        content << std::setw(0) << MarkerLocation.id << std::endl;
    }
    auto loc = _imgAnalysisDebugWindowLoc;
    DrawUIwindow("ImageAnalysis Data", content.str().c_str(), loc.x, loc.y, loc.z);
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

void Render::renderUIrenderingDebug(const int &trackSegmentCount) {
    std::stringstream content;
    content.setf(std::ios::fixed, std::ios::floatfield);
    content.setf(std::ios::showpoint); //Always show float decimal place
    content << trackSegmentCount << " Track segments";
    auto loc = _renderDebugWindowLoc;
    DrawUIwindow("Render info", content.str().c_str(), loc.x, loc.y, loc.z);
}

