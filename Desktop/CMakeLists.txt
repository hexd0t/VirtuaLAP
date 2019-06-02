cmake_minimum_required(VERSION 3.14)
project(VirtuaLAP-Desktop)

set(CMAKE_CXX_STANDARD 17)

include_directories(Dependencies/OpenCV/include)
include_directories(Dependencies/glfw/include)
include_directories(Dependencies/glad/include)
include_directories(Dependencies/libpng)
include_directories(Dependencies/libpngconf)

FILE(GLOB GlobalCode ../Global/*.cpp) #Build all cpp files from the Global folder
add_executable(VirtuaLAP-Desktop main.cpp UI.cpp Dependencies/glad/src/glad.c ${GlobalCode} ../Global/nanovg/src/nanovg.c)

#Find & link against OpenCV
set(OpenCV_STATIC ON)
FIND_PACKAGE(OpenCV REQUIRED PATHS Dependencies/opencv)
TARGET_LINK_LIBRARIES(VirtuaLAP-Desktop opencv_core opencv_imgproc opencv_imgcodecs opencv_aruco opencv_highgui)

#Build & link against GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Dependencies/glfw)
target_link_libraries(VirtuaLAP-Desktop glfw)

if (WIN32)
    SET_TARGET_PROPERTIES(VirtuaLAP-Desktop PROPERTIES LINK_FLAGS -mconsole)
endif()


