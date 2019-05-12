#pragma once

#include <functional>
#include "PipelineInterfaces.h"

typedef std::function<void (CameraImageData* result)> CaptureImageFunc;
