#pragma once

#include "PipelineInterfaces.h"

class ImageAnalysis {
private: //Class members

public:  //Public methods
    ImageAnalysis();
    ~ImageAnalysis();

    void Step(const CameraImageData* cameraImage,ImageAnalysisResult* result);
private: //Private methods

};

