#include "glm/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Render_Constants.h"
#include "Render_Shader.h"
#include "Render_OGL.h"

Shader::Shader() :
    _diffuseTextureSampler(UINT32_MAX),
    _shaderUniformModel(UINT32_MAX), _shaderUniformNormal(UINT32_MAX),
    _shaderUniformProj(UINT32_MAX), _shaderUniformView(UINT32_MAX)
{

}

Shader::~Shader() {

}

void Shader::Init(const char *sourceName) {
    glGenVertexArrays(1, &_vertexArray);
    glBindVertexArray(_vertexArray);

    auto source = std::string(sourceName);
    auto vertexShaderGLSL = readShaderSource( (source+".vs.glsl").c_str() );
    auto fragmentShaderGLSL = readShaderSource( (source+".fs.glsl").c_str() );
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
        std::cerr << "Error compiling Vertex shader "<<sourceName<<": " << &vertexShaderErrorMessage[0] << std::endl;
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
        std::cerr << "Error compiling Fragment shader "<<sourceName<<": " << &fragmentShaderErrorMessage[0] << std::endl;
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
        std::cerr << "Error linking shader program  "<<sourceName<<": " <<&shaderProgramErrorMessage[0] << std::endl;
        throw std::runtime_error( "Error linking shader program" );
    }
    glUseProgram( _shaderProgram );
    loadUniforms();
}

void Shader::Apply() {
    glUseProgram( _shaderProgram );
    glBindVertexArray(_vertexArray);

    uploadModel();
    uploadProj();
    uploadView();
}

std::vector<char> Shader::readShaderSource(const char *name) const {
    std::ifstream shaderFile( name, std::ios::binary );
    if (!shaderFile)
        throw std::runtime_error( "Cannot read Shader file" );

    shaderFile.seekg( 0, std::ifstream::end );
    int shaderLength = static_cast<int>(shaderFile.tellg());
    shaderFile.seekg( 0, std::ifstream::beg );

    std::vector<char> shaderGLSL( shaderLength + 1, '\0' );
    shaderFile.read( &shaderGLSL[0], shaderLength );

    shaderFile.close();

    return shaderGLSL;
}

void Shader::SetDiffuseTexture(unsigned int textureHandle) {

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glUniform1i( _diffuseTextureSampler, 0 );
}

void Shader::loadUniforms() {
    GLint total = -1;
    glGetProgramiv( _shaderProgram, GL_ACTIVE_UNIFORMS, &total );

    for (GLuint i = 0U; static_cast<GLint>(i)<total; ++i)  {
        GLint nameLength = -1;
        GLint size = -1;
        GLenum type = GL_ZERO;
        char uniformName[100];
        glGetActiveUniform( _shaderProgram, i, sizeof( uniformName ) - 1, &nameLength, &size, &type, uniformName );
        uniformName[nameLength] = 0; //Gültigen C-String erzeugen
        GLuint location = glGetUniformLocation( _shaderProgram, uniformName );
        std::string name( uniformName );

        if (name == "model")
            _shaderUniformModel = location;
        else if (name == "normal")
            _shaderUniformNormal = location;
        else if (name == "view")
            _shaderUniformView = location;
        else if (name == "proj")
            _shaderUniformProj = location;
        /*else if (name == "lightPos")
            shaderUniformLightPos = location;
        else if (name == "lightColor")
            shaderUniformLightColor = location;
        else if (name == "lightPower")
            shaderUniformLightPower = location;
        else if (name == "ambientLight")
            shaderUniformAmbientLight = location;
        else if (name == "specularColor")
            shaderUniformSpecularColor = location;
        else if (name == "specularExponent")
            shaderUniformSpecularExponent = location;
        else if (name == "heightTextureSampler")
            shaderUniformHeightTextureSampler = location;*/
        else if (name == "diffuseTextureSampler")
            _diffuseTextureSampler = location;
/*
        else if (name == "segmentSize")
            shaderUniformSegmentSize = location;
        else if (name == "sunDirection")
            shaderUniformSunDirection = location;
        else if (name == "sunColor")
            shaderUniformSunColor = location;
        else if (name == "fadeDistance")
            shaderUniformFadeDist = location;*/
        else
            std::cerr << "Unknown shader uniform " << name;
    }
}


void Shader::uploadModel()
{
    if (_shaderUniformModel < UINT32_MAX)
        glUniformMatrix4fv( _shaderUniformModel, 1, GL_FALSE, &_model[0][0] );
    if (_shaderUniformNormal < UINT32_MAX)
        glUniformMatrix3fv( _shaderUniformNormal, 1, GL_FALSE, &_normal[0][0] );
}

void Shader::uploadView()
{
    if (_shaderUniformView < UINT32_MAX)
        glUniformMatrix4fv( _shaderUniformView, 1, GL_FALSE, &_view[0][0] );
}

void Shader::uploadProj()
{
    if (_shaderUniformProj < UINT32_MAX)
        glUniformMatrix4fv( _shaderUniformProj, 1, GL_FALSE, &_proj[0][0] );
}

void Shader::UpdateModel(glm::mat4 transform) {
    _model = transform;
    _normal = glm::mat3( glm::transpose( glm::inverse( _view * _model ) ) );
    uploadModel();
}

void Shader::UpdateView(glm::mat4 transform) {
    _view = transform;
    uploadView();
}

void Shader::UpdateProj(glm::mat4 transform) {
    _proj = transform;
    //Since this happens outside of the drawing, don't upload here
}

