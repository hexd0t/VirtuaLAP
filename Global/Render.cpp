#include "Render.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <fstream>

Render::Render() :
    _aspectRatio(1.333f), _fov(45.f), _farDistance(5000)
{

}

Render::~Render() {

}

void Render::Step(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track, float deltaT, GameState* output) {
    glClearColor(0.8f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer( GL_ARRAY_BUFFER, _carVBO );
    Vertex::SetLayout();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Render::FramebufferSizeChanged(int width, int height) {
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

    shaderFile.seekg( 0, shaderFile.end );
    int shaderLength = static_cast<int>(shaderFile.tellg());
    shaderFile.seekg( 0, shaderFile.beg );

    std::vector<char> shaderGLSL( shaderLength + 1 );
    memset( &shaderGLSL[0], 0, shaderLength + 1 );
    shaderFile.read( &shaderGLSL[0], shaderLength );

    shaderFile.close();

    return shaderGLSL;
}

void Render::Init() {
    initShaders();
    initVBOs();
}

