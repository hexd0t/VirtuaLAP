#include "UI.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <mutex>
typedef std::recursive_mutex Mutex;
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

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

void UI::Init(std::function<void (int width, int height)> resizeCallback) {
    _resizeCallback = resizeCallback;
    if (!glfwInit()) {
        throw std::runtime_error("GLFW Init failed!");
    }
    _glfw_initialized = true;
    glfwSetErrorCallback(glfw_error_callback);

    createWindow();
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
    _window = glfwCreateWindow(RES_X, RES_Y, "VirtuaLAP Desktop", nullptr, nullptr);
    if(!_window) {
        throw std::runtime_error("GLFW window creation failed!");
    }
    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("GLAD loader init failed!");
    }
    glViewport(0, 0, RES_X, RES_Y);
    loadIcon();
    registerGLFWCallbacks();
}

void UI::Run(std::function<void (float dT)> draw_callback) {
    while (!glfwWindowShouldClose(_window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_callback(16.f);

        // Swap the screen buffers
        glfwSwapBuffers(_window);
    }
}

void UI::loadIcon() { //Abridged from http://zarb.org/~gc/html/libpng.html
    GLFWimage logo;
    auto cvimg = cv::imread("Logo.png", cv::IMREAD_COLOR);

    logo.height = cvimg.rows;
    logo.width = cvimg.cols;
    logo.pixels = cvimg.data;

    glfwSetWindowIcon(_window, 1, &logo); //copies the image data
}

void UI::CaptureImage(CameraImageData *result) {
    auto cvimg = cv::imread("test.jpg", cv::IMREAD_COLOR);
    //ToDo(AMü): Improve testing by allowing to choose different pictures using keyboard input
    // [See https://www.glfw.org/docs/latest/input_guide.html on how to get input]

    result->Height = cvimg.rows;
    result->Width = cvimg.cols;
    result->Data = new char[result->Height * result->Width * 3];
    //ToDo(AMü): Move buffer allocation to Global_Core, alloc only once

    auto target = result->Data;
    for(int y=0; y<result->Height; ++y) {
        memcpy(target, cvimg.ptr(y), result->Width*3);
        target += result->Width*3;
    }
}

void UI::KeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void UI::ResizeEvent(GLFWwindow *window, int width, int height) {
    _resizeCallback(width, height);
}

void UI::registerGLFWCallbacks() {
    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, &key_callback);
    glfwSetFramebufferSizeCallback(_window, &fbresize_callback);

    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( &oglDebug_callback, this );
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