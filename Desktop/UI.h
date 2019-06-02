#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../Global/PlatformInterfaces.h"

class UI {
private:
    bool _glfw_initialized = false;
    GLFWwindow* _window = nullptr;
    CoreCallbacks _callbacks;

    bool _debugWireframe = false;
    int _currentDebugImage = 0;
    cv::Mat _inputImg;
public:
    UI();
    ~UI();
    void Init(CoreCallbacks callbacks);
    void Deinit();

    void Run(const std::function<void (float dT)>& draw_callback);
    void CaptureImage(CameraImageData* result);

    void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ResizeEvent(GLFWwindow* window, int width, int height);
private:
    void createWindow();
    void registerGLFWCallbacks();
    void loadIcon();

    void drawDebugUI();
    void cycleDebugImage(int direction);
};

//C Callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void fbresize_callback(GLFWwindow* window, int width, int height);
void APIENTRY oglDebug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

