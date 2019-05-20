#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include "../Global/PlatformInterfaces.h"

class UI {
private:
    bool _glfw_initialized = false;
    GLFWwindow* _window = nullptr;
    std::function<void (int width, int height)> _resizeCallback;
public:
    UI();
    ~UI();
    void Init(std::function<void (int width, int height)> resizeCallback);
    void Deinit();

    void Run(std::function<void (float dT)> draw_callback);
    void CaptureImage(CameraImageData* result);

    void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ResizeEvent(GLFWwindow* window, int width, int height);
private:
    void createWindow();
    void registerGLFWCallbacks();
    void loadIcon();
};

//C Callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void fbresize_callback(GLFWwindow* window, int width, int height);
void APIENTRY oglDebug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

