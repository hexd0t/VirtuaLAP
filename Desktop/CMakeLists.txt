cmake_minimum_required(VERSION 3.14)
project(VirtuaLAP-Desktop)

set(CMAKE_CXX_STANDARD 14)

include_directories(Dependencies/OpenCV/include)
include_directories(Dependencies/glfw/include)
include_directories(Dependencies/glad/include)
include_directories(Dependencies/libpng)
include_directories(Dependencies/libpngconf)

FILE(GLOB GlobalCode ../Global/*.cpp) #Build all cpp files from the Global folder
add_executable(VirtuaLAP-Desktop main.cpp UI.cpp Dependencies/glad/src/glad.c ${GlobalCode})

#Link against prebuilt OpenCV
find_library(LIB_OPENCV NAMES opencv_world410d HINTS "Dependencies/OpenCV/lib")
message(STATUS "find_library opencv_world410d: ${LIB_OPENCV}")
target_link_libraries(VirtuaLAP-Desktop ${LIB_OPENCV})

#Build & link against GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Dependencies/glfw)
target_link_libraries(VirtuaLAP-Desktop glfw)

#Build & link against libpng (including zlib)
add_subdirectory(Dependencies/zlib EXCLUDE_FROM_ALL)
SET(ZLIB_LIBRARY zlibstatic)
set(ZLIB_INCLUDE_DIR Dependencies/zlib)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON)
add_subdirectory(Dependencies/libpng EXCLUDE_FROM_ALL)
target_link_libraries(VirtuaLAP-Desktop png_static)

