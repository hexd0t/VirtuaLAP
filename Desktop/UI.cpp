#include "UI.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <png.h>
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

void UI::Init() {
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
}

void UI::Run(std::function<void ()> draw_callback) {
    while (!glfwWindowShouldClose(_window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_callback();

        // Swap the screen buffers
        glfwSwapBuffers(_window);
    }
}

void UI::loadIcon() { //Abridged from http://zarb.org/~gc/html/libpng.html
    char header[8];
    GLFWimage logo;
    FILE *logofile = fopen("Logo.png", "rb");
    if (!logofile)
        throw std::runtime_error("Logo could not be opened for reading");
    fread(header, 1, 8, logofile);

    if(png_sig_cmp((png_const_bytep)header,0,8))
        throw std::runtime_error("Icon file has invalid magic numbers");

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
        throw std::runtime_error("Error during init_io");

    png_init_io(png_ptr, logofile);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    logo.width = png_get_image_width(png_ptr, info_ptr);
    logo.height = png_get_image_height(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
        throw std::runtime_error("Error during read_image");

    auto row_pointers = new png_bytep[sizeof(png_bytep) * logo.height];
    size_t datalen = 0;
    for (int y=0; y < logo.height; y++) {
        size_t rowlen = png_get_rowbytes(png_ptr, info_ptr);
        row_pointers[y] = (png_bytep)datalen;
        datalen += rowlen;
    }
    auto data = new png_byte[datalen]; //intentionally leak this, since we won't need it anymore
    for(int y=0; y < logo.height; y++)
        row_pointers[y] = data + (size_t)row_pointers[y];

    png_read_image(png_ptr, row_pointers);
    delete[] row_pointers;
    fclose(logofile);

    logo.pixels = data;

    glfwSetWindowIcon(_window, 1, &logo); //copies the image data
    delete[] data;
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
