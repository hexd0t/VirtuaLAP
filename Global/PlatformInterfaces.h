#pragma once

#include <functional>
#include "PipelineInterfaces.h"

// Calls from Global to Platform:
typedef std::function<void (CameraImageData* result)> CaptureImageFunc;

// Calls from Platform to Global:
typedef std::function<void (const char *title, const char *content, float x, float y, float w)> DrawUIWindowFunc;
typedef std::function<void (int width, int height)> ResizeFunc;