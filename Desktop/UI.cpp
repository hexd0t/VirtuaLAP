#include <utility>

#include "UI.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <mutex>
typedef std::recursive_mutex Mutex;

#define RES_X 1280
#define RES_Y 768

//This needs to be a c-style function
void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

UI::UI() {

}

UI::~UI() {
}

void UI::Init(CoreCallbacks callbacks) {
    _callbacks = std::move(callbacks);
    if (!glfwInit()) {
        throw std::runtime_error("GLFW Init failed!");
    }
    _glfw_initialized = true;
    glfwSetErrorCallback(glfw_error_callback);

    createWindow();
    cycleDebugImage(0); //Load test0.jpg
}

void UI::Deinit() {
    if(_window)
        glfwDestroyWindow(_window);
    if(_glfw_initialized)
        glfwTerminate();
}

void UI::createWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
    _window = glfwCreateWindow(RES_X, RES_Y, "VirtuaLAP Desktop", nullptr, nullptr);
    if(!_window) {
        throw std::runtime_error("GLFW window creation failed!");
    }
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("GLAD loader init failed!");
    }
    glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    glViewport(0, 0, RES_X, RES_Y);
    loadIcon();
    registerGLFWCallbacks();
}

void UI::Run(const std::function<void (float dT)>& draw_callback) {
    while (!glfwWindowShouldClose(_window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        draw_callback(16.f);

        drawDebugUI();

        // Swap the screen buffers
        glfwSwapBuffers(_window);
    }
}

void UI::loadIcon() {
    GLFWimage logo;
    auto cvimg = cv::imread("Logo.png", cv::IMREAD_COLOR);

    logo.height = cvimg.rows;
    logo.width = cvimg.cols;
    logo.pixels = cvimg.data;

    glfwSetWindowIcon(_window, 1, &logo); //copies the image data
}

void UI::CaptureImage(CameraImageData *result) {

    static bool init = true;
    if(init) {
        int oldid = _currentDebugImage;
        cycleDebugImage(1);
        if(oldid == _currentDebugImage) {
            init = false;
            _currentDebugImage = 0;
            cycleDebugImage(0);
        }
    }

    result->Height = _inputImg.rows;
    result->Width = _inputImg.cols;
    result->Data = new char[result->Height * result->Width * 3];
    //ToDo(AMü): Move buffer allocation to Global_Core, alloc only once

    auto target = result->Data;
    for(int y=0; y<result->Height; ++y) {
        memcpy(target, _inputImg.ptr(y), result->Width*3);
        target += result->Width*3;
    }
}

void UI::KeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_F1:
                _callbacks.SimulateImgAnalysis();
                break;
            case GLFW_KEY_F2:
                _callbacks.ToggleImgAnalysisDebug();
                break;
            case GLFW_KEY_F3:
                cycleDebugImage(-1);
                break;
            case GLFW_KEY_F4:
                cycleDebugImage(1);
                break;
            case GLFW_KEY_F5:
                _debugWireframe = !_debugWireframe;
                if (_debugWireframe)
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                else
                    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
            case GLFW_KEY_F12:
                _callbacks.DisplayMarkers();
                break;
            default:
                break;
        }

    }
}

void UI::ResizeEvent(GLFWwindow *window, int width, int height) {
    _callbacks.Resize(width, height);
}

void UI::registerGLFWCallbacks() {
    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, &key_callback);
    glfwSetFramebufferSizeCallback(_window, &fbresize_callback);

    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( &oglDebug_callback, this );
}

void UI::drawDebugUI() {
    std::stringstream text;
    text << "In: test" << _currentDebugImage << ".jpg [F3/F4]\n";
    text << "F1: Sim ImgAnal.\n";
    text << "F2: Debug ImgAnal.\n";

    _callbacks.DrawUIWindow("Debug", text.str().c_str(), 20, 20, 200);
}

void UI::cycleDebugImage(int direction) {
    int newindex = _currentDebugImage + direction;
    auto img = cv::imread("test"+std::to_string(newindex)+".jpg", cv::IMREAD_COLOR);
    if(img.empty())
        return; //File not readable
    cv::cvtColor(img, _inputImg, cv::COLOR_BGR2RGB);
    //_inputImg = img;
    _currentDebugImage = newindex;
}


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    //Don't implement anything here, just pass the call through to the member function
    UI* uiInstance = static_cast<UI*>(glfwGetWindowUserPointer( window ));
    uiInstance->KeyEvent( window, key, scancode, action, mods );
}

void fbresize_callback(GLFWwindow *window, int width, int height) {
    UI* uiInstance = static_cast<UI*>(glfwGetWindowUserPointer( window ));
    uiInstance->ResizeEvent( window, width, height );
}

void APIENTRY oglDebug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if (id == GL_INVALID_OPERATION)
        std::cerr <<  "OGL Error: " << std::string( message, message + length ) << std::endl;
    else {
        if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
            std::cout << "OGL Info: " << std::string(message, message + length) << std::endl;
    }
}