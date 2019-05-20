#include "Render.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"

Render::Render() :
    _outputWidthPx(1280), _outputHeightPx(768),
    _aspectRatio(1.333f), _fov(45.f), _farDistance(5000)
{

}

Render::~Render() {

}

void Render::Step(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* gameState) {
    glClearColor(0.8f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(_shaderProgram);
    glBindVertexArray(_vertexArray);
    glBindBuffer( GL_ARRAY_BUFFER, _carVBO );
    Vertex::SetLayout();

    glDrawArrays(GL_TRIANGLES, 0, 3);


    renderUI(camImage, imgAnalysis, track, deltaT, gameState);
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
    glGenVertexArrays(1, &_vertexArray); //Note(AMü): if we use multiple shaders, each might need their own
    glBindVertexArray(_vertexArray);

    auto vertexShaderGLSL = readShaderSource( "default.vs.glsl");
    auto fragmentShaderGLSL = readShaderSource( "default.fs.glsl");
    char* vertexShaderGLSLptr = &vertexShaderGLSL[0];
    char* fragmentShaderGLSLptr = &fragmentShaderGLSL[0];

    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource( _vertexShader, 1, &vertexShaderGLSLptr, nullptr );
    glCompileShader( _vertexShader );

    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource( _fragmentShader, 1, &fragmentShaderGLSLptr, nullptr );
    glCompileShader( _fragmentShader );

    GLint result = GL_FALSE;

    glGetShaderiv( _vertexShader, GL_COMPILE_STATUS, &result );
    if (result != GL_TRUE) {
        int infoLogLength = -1;
        glGetShaderiv( _vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength );
        if (infoLogLength < 1)
            throw std::runtime_error( "Unable to compile VS, no error message" );
        std::vector<char> vertexShaderErrorMessage( infoLogLength );
        glGetShaderInfoLog( _vertexShader, infoLogLength, nullptr, &vertexShaderErrorMessage[0] );
        std::cerr << "Error compiling Vertex shader: " << &vertexShaderErrorMessage[0] << std::endl;
        throw std::runtime_error( "Unable to compile VS" );
    }

    glGetShaderiv( _fragmentShader, GL_COMPILE_STATUS, &result );
    if (result != GL_TRUE) {
        int infoLogLength = -1;
        glGetShaderiv( _fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength );
        if (infoLogLength < 1)
            throw std::runtime_error( "Unable to compile FS, no error message" );
        std::vector<char> fragmentShaderErrorMessage( infoLogLength );
        glGetShaderInfoLog( _fragmentShader, infoLogLength, nullptr, &fragmentShaderErrorMessage[0] );
        std::cerr << "Error compiling Fragment shader: " << &fragmentShaderErrorMessage[0] << std::endl;
        throw std::runtime_error( "Unable to compile FS" );
    }

    _shaderProgram = glCreateProgram();
    glAttachShader( _shaderProgram, _vertexShader );
    glAttachShader( _shaderProgram, _fragmentShader );
    glLinkProgram( _shaderProgram );

    glGetProgramiv( _shaderProgram, GL_LINK_STATUS, &result );
    if (result != GL_TRUE) {
        int infoLogLength = -1;
        glGetProgramiv( _shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength );
        if (infoLogLength < 1)
            throw std::runtime_error( "Unable to link program, no error message" );
        std::vector<char> shaderProgramErrorMessage( infoLogLength );
        glGetProgramInfoLog( _shaderProgram, infoLogLength, nullptr, &shaderProgramErrorMessage[0] );
        std::cerr << "Error linking shader program: " <<&shaderProgramErrorMessage[0] << std::endl;
        throw std::runtime_error( "Error linking shader program" );
    }
    glUseProgram( _shaderProgram );
}

void Render::initVBOs() {
    std::vector<Vertex> carVertices;
    carVertices.emplace_back(-0.8f, -0.8f,  0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    carVertices.emplace_back(  0.f,  0.8f,  0.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    carVertices.emplace_back( 0.8f, -0.8f, 0.2f, 0.f, 0.f, 1.f, 0.f, 1.f);

    _carVBO = createVertexBuffer(carVertices);
}

unsigned int Render::createVertexBuffer(const std::vector<Vertex>& vertices) const {
    GLuint vertexBuffer;
    glGenBuffers( 1, &vertexBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), &vertices[0], GL_STATIC_DRAW );
    return vertexBuffer;
}

std::vector<char> Render::readShaderSource(const char *name) const {
    std::ifstream shaderFile( name, std::ios::binary );
    if (!shaderFile)
        throw std::runtime_error( "Cannot read Shader file" );

    shaderFile.seekg( 0, std::ifstream::end );
    int shaderLength = static_cast<int>(shaderFile.tellg());
    shaderFile.seekg( 0, std::ifstream::beg );

    std::vector<char> shaderGLSL( shaderLength + 1 );
    memset( &shaderGLSL[0], 0, shaderLength + 1 );
    shaderFile.read( &shaderGLSL[0], shaderLength );

    shaderFile.close();

    return shaderGLSL;
}

void Render::Init() {
    initShaders();
    initVBOs();
    initUI();
}

void Render::renderUI(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* gameState) {
    nvgBeginFrame(vg, _outputWidthPx, _outputHeightPx, 1.0f);

    //Image Analysis Debug Window
    int iadw_x = 20;
    int iadw_y = 20;
    int iadw_w = 200;
    int iadw_h = 400;
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
    drawUIwindow("ImageAnalysis Data", content.str().c_str(), iadw_x, iadw_y, iadw_w);

    nvgEndFrame(vg);
}

void Render::drawUIcontent(const char *content, float x, float y, float w, float h) {
    const int margin_left = 8;
    const int margin_top = 40;
    nvgFontSize(vg, 16.0f);
    nvgFontFace(vg, "arial");
    nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);

    nvgFontBlur(vg,0);
    nvgFillColor(vg, nvgRGBA(200,200,200,180));
    nvgTextBox(vg, x + margin_left, y+margin_top, w - 2*margin_left, content, nullptr);
}

void Render::drawUIwindowBorder(const char *title, float x, float y, float w, float h) {
    const float cornerRadius = 5.0f;
    NVGpaint shadowPaint;
    NVGpaint headerPaint;

    nvgSave(vg);

    // Window
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x,y, w,h, cornerRadius);
    nvgFillColor(vg, nvgRGBA(28,30,34,192));
    nvgFill(vg);

    // Drop shadow
    shadowPaint = nvgBoxGradient(vg, x,y+2, w,h, cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
    nvgBeginPath(vg);
    nvgRect(vg, x-10,y-10, w+20,h+30);
    nvgRoundedRect(vg, x,y, w,h, cornerRadius);
    nvgPathWinding(vg, NVG_HOLE);
    nvgFillPaint(vg, shadowPaint);
    nvgFill(vg);

    // Header
    headerPaint = nvgLinearGradient(vg, x,y,x,y+15, nvgRGBA(255,255,255,8), nvgRGBA(0,0,0,16));
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x+1,y+1, w-2,30, cornerRadius-1);
    nvgFillPaint(vg, headerPaint);
    nvgFill(vg);
    nvgBeginPath(vg);
    nvgMoveTo(vg, x+0.5f, y+0.5f+30);
    nvgLineTo(vg, x+0.5f+w-1, y+0.5f+30);
    nvgStrokeColor(vg, nvgRGBA(0,0,0,32));
    nvgStroke(vg);

    nvgFontSize(vg, 18.0f);
    nvgFontFace(vg, "arial");
    nvgTextAlign(vg,NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);

    nvgFontBlur(vg,2);
    nvgFillColor(vg, nvgRGBA(0,0,0,128));
    nvgText(vg, x+w/2,y+16+1, title, nullptr);

    nvgFontBlur(vg,0);
    nvgFillColor(vg, nvgRGBA(220,220,220,160));
    nvgText(vg, x+w/2,y+16, title, nullptr);

    nvgRestore(vg);
}

void Render::initUI() {
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

    //Note(AMü): Since these will never be unloaded, we don't store the handles returned
    nvgCreateFont(vg, "arial", "C:/Windows/Fonts/arial.ttf");
}

void Render::drawUIwindow(const char *title, const char *content, float x, float y, float w) {
    float contentbounds[4] = {0.f};
    nvgTextBoxBounds(vg, x, y, w - 16, content, nullptr, contentbounds);
    float windowheight = contentbounds[3] - contentbounds[1] + 48;
    drawUIwindowBorder(title, x, y, w, windowheight);
    drawUIcontent(content, x, y, w, windowheight);
}

