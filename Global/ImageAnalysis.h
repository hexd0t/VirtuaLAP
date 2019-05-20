#pragma once

#include "PipelineInterfaces.h"
//#include <opencv2/aruco.hpp>

class ImageAnalysis {
private: //Class members
    ImageAnalysisState _state;
public:  //Public methods
    ImageAnalysis();
    ~ImageAnalysis();

    void Step(const CameraImageData* cameraImage,ImageAnalysisResult* result);
private: //Private methods

};

