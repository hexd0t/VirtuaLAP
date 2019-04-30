cmake_minimum_required(VERSION 3.14)
project(VirtuaLAP)
option(BUILD_DESKTOP "Build Desktop Executable" ON)

if(BUILD_DESKTOP)
    add_subdirectory(Desktop)
endif()