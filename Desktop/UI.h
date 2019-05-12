#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include "../Global/PlatformInterfaces.h"

class UI {
private:
    bool _glfw_initialized = false;
    GLFWwindow* _window = nullptr;
public:
    UI();
    ~UI();
    void Init();
    void Deinit();

    void Run(std::function<void ()> draw_callback);
    void CaptureImage(CameraImageData* result);
private:
    void createWindow();
    void loadIcon();
};


